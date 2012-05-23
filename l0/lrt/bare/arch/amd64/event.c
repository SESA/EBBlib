/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <arch/amd64/apic.h>
#include <arch/amd64/idt.h>
#include <arch/amd64/pic.h>
#include <arch/amd64/pit.h>
#include <arch/amd64/rtc.h>
#include <arch/amd64/segmentation.h>
#include <l0/lrt/event.h>
#include <l0/lrt/mem.h>
#include <l0/lrt/bare/arch/amd64/acpi.h>
#include <l0/lrt/bare/arch/amd64/lrt_start.h>
#include <lrt/io.h>

static int num_event_loc;

lrt_event_loc
lrt_num_event_loc()
{
  return num_event_loc;
}

lrt_event_loc
lrt_next_event_loc(lrt_event_loc l)
{
  return 0;
}

static lrt_event_loc bsp_loc;

void
lrt_event_set_bsp(lrt_event_loc loc)
{
  bsp_loc = loc;
}

lrt_event_loc
lrt_event_bsp_loc()
{
  return bsp_loc;
}
static struct lrt_event_descriptor lrt_event_table[LRT_EVENT_NUM_EVENTS];

static idtdesc *idt;

static inline void
idt_map_vec(uint8_t vec, void *addr) {
  idt[vec].offset_low = ((uintptr_t)addr) & 0xFFFF;
  idt[vec].offset_high = ((uintptr_t)addr) >> 16;
  idt[vec].selector = 0x8; //Our code segment
  idt[vec].ist = 0; //no stack switch
  idt[vec].type = 0xe;
  idt[vec].p = 1; //present
}

static void *isrtbl[];

static inline void
init_idt(void)
{
  for (int i = 0; i < 256; i++) {
    idt_map_vec(i, isrtbl[i]);
  }
}

void
lrt_event_preinit(int cores)
{
  num_event_loc = cores;
  idt = lrt_mem_alloc(sizeof(idtdesc) * 256, 8, lrt_event_bsp_loc());
  init_idt();
  LRT_Assert(has_lapic());
  disable_pic();
  //Disable the pit, irq 0 could have fired and therefore wouldn't
  //have been masked and then we enable interrupts so we must reset
  //the PIT (and we may as well prevent it from firing)
  disable_pit();

  //Disable the rtc, irq 8 could have fired and therefore wouldn't
  //have been masked and then we enable interrupts so we must disable
  //it
  disable_rtc();
}

void __attribute__ ((noreturn))
lrt_event_loop(void)
{
  //After we enable interrupts we just halt, an interrupt should wake
  //us up. Once we finish the interrupt, we halt again and repeat

  __asm__ volatile("sti"); //enable interrupts
  while (1) {
    __asm__ volatile("hlt");
  }
}

volatile int smp_lock;

void *
lrt_event_init(void *unused)
{
  load_idtr(idt, sizeof(idtdesc) * 256);

  enable_lapic();

  lrt_event_loc loc = get_lapic_id();

  lrt_event_loc *myloc = lrt_mem_alloc(sizeof(lrt_event_loc),
                                       sizeof(lrt_event_loc),
                                       loc);

  *myloc = loc;

  asm volatile (
                "wrmsr"
                :
                : "d" ((uintptr_t)myloc >> 32),
                  "a" ((uintptr_t)myloc),
                  "c" (0xc0000101));

  // call lrt_start before entering the loop
  // We switch stacks here to our own dynamically allocated stack
#define STACK_SIZE (1 << 14)
  char *myStack = lrt_mem_alloc(STACK_SIZE, 16, lrt_my_event_loc());

  asm volatile (
                "mov %[stack], %%rsp\n\t"
                "movl $0x0, %[smp_lock]\n\t"
                "call lrt_start"
                :
                : [stack] "r" (&myStack[STACK_SIZE]),
                  [smp_lock] "m" (smp_lock)
                );
  lrt_event_loop();
}

void
lrt_event_bind_event(lrt_event_num num, lrt_trans_id handler,
                     lrt_trans_func_num fnum)
{
  lrt_event_table[num].id = handler;
  lrt_event_table[num].fnum = fnum;
}

void
lrt_event_trigger_event(lrt_event_num num, enum lrt_event_loc_desc desc,
                        lrt_event_loc loc)
{
  lapic_icr_low icr_low;
  icr_low.raw = 0;
  icr_low.vector = num + 32;
  icr_low.level = 1;

  lapic_icr_high icr_high;
  icr_high.raw = 0;
  if (desc == LRT_EVENT_LOC_SINGLE) {
    icr_low.destination_shorthand = 0;
    icr_high.destination = loc;
  } else {
    LRT_Assert(0);
  }

  send_ipi(icr_low, icr_high);
}

void lrt_event_route_irq(struct IRQ_t *isrc, lrt_event_num num,
                         enum lrt_event_loc_desc desc, lrt_event_loc loc)
{
  LRT_Assert(0);
}

void
exception_common(uint8_t num) {
  lrt_printf("Received exception %d\n!", num);
  LRT_Assert(0);
}

void
event_common(uint8_t num) {
  send_eoi();
  uint8_t ev = num - 32; //first 32 interrupts are reserved
  struct lrt_event_descriptor *desc = &lrt_event_table[ev];
  lrt_trans_id id = desc->id;
  lrt_trans_func_num fnum = desc->fnum;

  //this infrastructure should be pulled out of this file
  lrt_trans_rep_ref ref = lrt_trans_id_dref(id);
  ref->ft[fnum](ref);
}

extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);
extern void isr_32(void);
extern void isr_33(void);
extern void isr_34(void);
extern void isr_35(void);
extern void isr_36(void);
extern void isr_37(void);
extern void isr_38(void);
extern void isr_39(void);
extern void isr_40(void);
extern void isr_41(void);
extern void isr_42(void);
extern void isr_43(void);
extern void isr_44(void);
extern void isr_45(void);
extern void isr_46(void);
extern void isr_47(void);
extern void isr_48(void);
extern void isr_49(void);
extern void isr_50(void);
extern void isr_51(void);
extern void isr_52(void);
extern void isr_53(void);
extern void isr_54(void);
extern void isr_55(void);
extern void isr_56(void);
extern void isr_57(void);
extern void isr_58(void);
extern void isr_59(void);
extern void isr_60(void);
extern void isr_61(void);
extern void isr_62(void);
extern void isr_63(void);
extern void isr_64(void);
extern void isr_65(void);
extern void isr_66(void);
extern void isr_67(void);
extern void isr_68(void);
extern void isr_69(void);
extern void isr_70(void);
extern void isr_71(void);
extern void isr_72(void);
extern void isr_73(void);
extern void isr_74(void);
extern void isr_75(void);
extern void isr_76(void);
extern void isr_77(void);
extern void isr_78(void);
extern void isr_79(void);
extern void isr_80(void);
extern void isr_81(void);
extern void isr_82(void);
extern void isr_83(void);
extern void isr_84(void);
extern void isr_85(void);
extern void isr_86(void);
extern void isr_87(void);
extern void isr_88(void);
extern void isr_89(void);
extern void isr_90(void);
extern void isr_91(void);
extern void isr_92(void);
extern void isr_93(void);
extern void isr_94(void);
extern void isr_95(void);
extern void isr_96(void);
extern void isr_97(void);
extern void isr_98(void);
extern void isr_99(void);
extern void isr_100(void);
extern void isr_101(void);
extern void isr_102(void);
extern void isr_103(void);
extern void isr_104(void);
extern void isr_105(void);
extern void isr_106(void);
extern void isr_107(void);
extern void isr_108(void);
extern void isr_109(void);
extern void isr_110(void);
extern void isr_111(void);
extern void isr_112(void);
extern void isr_113(void);
extern void isr_114(void);
extern void isr_115(void);
extern void isr_116(void);
extern void isr_117(void);
extern void isr_118(void);
extern void isr_119(void);
extern void isr_120(void);
extern void isr_121(void);
extern void isr_122(void);
extern void isr_123(void);
extern void isr_124(void);
extern void isr_125(void);
extern void isr_126(void);
extern void isr_127(void);
extern void isr_128(void);
extern void isr_129(void);
extern void isr_130(void);
extern void isr_131(void);
extern void isr_132(void);
extern void isr_133(void);
extern void isr_134(void);
extern void isr_135(void);
extern void isr_136(void);
extern void isr_137(void);
extern void isr_138(void);
extern void isr_139(void);
extern void isr_140(void);
extern void isr_141(void);
extern void isr_142(void);
extern void isr_143(void);
extern void isr_144(void);
extern void isr_145(void);
extern void isr_146(void);
extern void isr_147(void);
extern void isr_148(void);
extern void isr_149(void);
extern void isr_150(void);
extern void isr_151(void);
extern void isr_152(void);
extern void isr_153(void);
extern void isr_154(void);
extern void isr_155(void);
extern void isr_156(void);
extern void isr_157(void);
extern void isr_158(void);
extern void isr_159(void);
extern void isr_160(void);
extern void isr_161(void);
extern void isr_162(void);
extern void isr_163(void);
extern void isr_164(void);
extern void isr_165(void);
extern void isr_166(void);
extern void isr_167(void);
extern void isr_168(void);
extern void isr_169(void);
extern void isr_170(void);
extern void isr_171(void);
extern void isr_172(void);
extern void isr_173(void);
extern void isr_174(void);
extern void isr_175(void);
extern void isr_176(void);
extern void isr_177(void);
extern void isr_178(void);
extern void isr_179(void);
extern void isr_180(void);
extern void isr_181(void);
extern void isr_182(void);
extern void isr_183(void);
extern void isr_184(void);
extern void isr_185(void);
extern void isr_186(void);
extern void isr_187(void);
extern void isr_188(void);
extern void isr_189(void);
extern void isr_190(void);
extern void isr_191(void);
extern void isr_192(void);
extern void isr_193(void);
extern void isr_194(void);
extern void isr_195(void);
extern void isr_196(void);
extern void isr_197(void);
extern void isr_198(void);
extern void isr_199(void);
extern void isr_200(void);
extern void isr_201(void);
extern void isr_202(void);
extern void isr_203(void);
extern void isr_204(void);
extern void isr_205(void);
extern void isr_206(void);
extern void isr_207(void);
extern void isr_208(void);
extern void isr_209(void);
extern void isr_210(void);
extern void isr_211(void);
extern void isr_212(void);
extern void isr_213(void);
extern void isr_214(void);
extern void isr_215(void);
extern void isr_216(void);
extern void isr_217(void);
extern void isr_218(void);
extern void isr_219(void);
extern void isr_220(void);
extern void isr_221(void);
extern void isr_222(void);
extern void isr_223(void);
extern void isr_224(void);
extern void isr_225(void);
extern void isr_226(void);
extern void isr_227(void);
extern void isr_228(void);
extern void isr_229(void);
extern void isr_230(void);
extern void isr_231(void);
extern void isr_232(void);
extern void isr_233(void);
extern void isr_234(void);
extern void isr_235(void);
extern void isr_236(void);
extern void isr_237(void);
extern void isr_238(void);
extern void isr_239(void);
extern void isr_240(void);
extern void isr_241(void);
extern void isr_242(void);
extern void isr_243(void);
extern void isr_244(void);
extern void isr_245(void);
extern void isr_246(void);
extern void isr_247(void);
extern void isr_248(void);
extern void isr_249(void);
extern void isr_250(void);
extern void isr_251(void);
extern void isr_252(void);
extern void isr_253(void);
extern void isr_254(void);
extern void isr_255(void);

static void *isrtbl[] = {
  isr_0,
  isr_1,
  isr_2,
  isr_3,
  isr_4,
  isr_5,
  isr_6,
  isr_7,
  isr_8,
  isr_9,
  isr_10,
  isr_11,
  isr_12,
  isr_13,
  isr_14,
  isr_15,
  isr_16,
  isr_17,
  isr_18,
  isr_19,
  isr_20,
  isr_21,
  isr_22,
  isr_23,
  isr_24,
  isr_25,
  isr_26,
  isr_27,
  isr_28,
  isr_29,
  isr_30,
  isr_31,
  isr_32,
  isr_33,
  isr_34,
  isr_35,
  isr_36,
  isr_37,
  isr_38,
  isr_39,
  isr_40,
  isr_41,
  isr_42,
  isr_43,
  isr_44,
  isr_45,
  isr_46,
  isr_47,
  isr_48,
  isr_49,
  isr_50,
  isr_51,
  isr_52,
  isr_53,
  isr_54,
  isr_55,
  isr_56,
  isr_57,
  isr_58,
  isr_59,
  isr_60,
  isr_61,
  isr_62,
  isr_63,
  isr_64,
  isr_65,
  isr_66,
  isr_67,
  isr_68,
  isr_69,
  isr_70,
  isr_71,
  isr_72,
  isr_73,
  isr_74,
  isr_75,
  isr_76,
  isr_77,
  isr_78,
  isr_79,
  isr_80,
  isr_81,
  isr_82,
  isr_83,
  isr_84,
  isr_85,
  isr_86,
  isr_87,
  isr_88,
  isr_89,
  isr_90,
  isr_91,
  isr_92,
  isr_93,
  isr_94,
  isr_95,
  isr_96,
  isr_97,
  isr_98,
  isr_99,
  isr_100,
  isr_101,
  isr_102,
  isr_103,
  isr_104,
  isr_105,
  isr_106,
  isr_107,
  isr_108,
  isr_109,
  isr_110,
  isr_111,
  isr_112,
  isr_113,
  isr_114,
  isr_115,
  isr_116,
  isr_117,
  isr_118,
  isr_119,
  isr_120,
  isr_121,
  isr_122,
  isr_123,
  isr_124,
  isr_125,
  isr_126,
  isr_127,
  isr_128,
  isr_129,
  isr_130,
  isr_131,
  isr_132,
  isr_133,
  isr_134,
  isr_135,
  isr_136,
  isr_137,
  isr_138,
  isr_139,
  isr_140,
  isr_141,
  isr_142,
  isr_143,
  isr_144,
  isr_145,
  isr_146,
  isr_147,
  isr_148,
  isr_149,
  isr_150,
  isr_151,
  isr_152,
  isr_153,
  isr_154,
  isr_155,
  isr_156,
  isr_157,
  isr_158,
  isr_159,
  isr_160,
  isr_161,
  isr_162,
  isr_163,
  isr_164,
  isr_165,
  isr_166,
  isr_167,
  isr_168,
  isr_169,
  isr_170,
  isr_171,
  isr_172,
  isr_173,
  isr_174,
  isr_175,
  isr_176,
  isr_177,
  isr_178,
  isr_179,
  isr_180,
  isr_181,
  isr_182,
  isr_183,
  isr_184,
  isr_185,
  isr_186,
  isr_187,
  isr_188,
  isr_189,
  isr_190,
  isr_191,
  isr_192,
  isr_193,
  isr_194,
  isr_195,
  isr_196,
  isr_197,
  isr_198,
  isr_199,
  isr_200,
  isr_201,
  isr_202,
  isr_203,
  isr_204,
  isr_205,
  isr_206,
  isr_207,
  isr_208,
  isr_209,
  isr_210,
  isr_211,
  isr_212,
  isr_213,
  isr_214,
  isr_215,
  isr_216,
  isr_217,
  isr_218,
  isr_219,
  isr_220,
  isr_221,
  isr_222,
  isr_223,
  isr_224,
  isr_225,
  isr_226,
  isr_227,
  isr_228,
  isr_229,
  isr_230,
  isr_231,
  isr_232,
  isr_233,
  isr_234,
  isr_235,
  isr_236,
  isr_237,
  isr_238,
  isr_239,
  isr_240,
  isr_241,
  isr_242,
  isr_243,
  isr_244,
  isr_245,
  isr_246,
  isr_247,
  isr_248,
  isr_249,
  isr_250,
  isr_251,
  isr_252,
  isr_253,
  isr_254,
  isr_255
};
