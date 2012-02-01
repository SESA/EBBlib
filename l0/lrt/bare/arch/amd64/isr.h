#ifndef L0_LRT_BARE_ARCH_AMD64_ISR_H
#define L0_LRT_BARE_ARCH_AMD64_ISR_H
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

#include <stdint.h>

/**
 * isr stub functions, defined in isr.S. install irsn in slot n in the idt.
 */
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

/**
 * common interrupt handler. defined in isr_common.c.
 * this is called by each of the above stub functions
 * with the apropriate interrupt number.
 */
extern void isr_common(uint64_t int_no);

/* in isrtbl.c */
extern void *isrtbl[];

#endif
