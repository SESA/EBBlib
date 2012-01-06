#ifndef __AMD64_ISR_H_
#define __AMD64_ISR_H_
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
void isr0(void);
void isr1(void);
void isr2(void);
void isr3(void);
void isr4(void);
void isr5(void);
void isr6(void);
void isr7(void);
void isr8(void);
void isr9(void);
void isr10(void);
void isr11(void);
void isr12(void);
void isr13(void);
void isr14(void);
void isr15(void);
void isr16(void);
void isr17(void);
void isr18(void);
void isr19(void);
void isr20(void);
void isr21(void);
void isr22(void);
void isr23(void);
void isr24(void);
void isr25(void);
void isr26(void);
void isr27(void);
void isr28(void);
void isr29(void);
void isr30(void);
void isr31(void);
void isr32(void);
void isr33(void);
void isr34(void);
void isr35(void);
void isr36(void);
void isr37(void);
void isr38(void);
void isr39(void);
void isr40(void);
void isr41(void);
void isr42(void);
void isr43(void);
void isr44(void);
void isr45(void);
void isr46(void);
void isr47(void);
void isr48(void);
void isr49(void);
void isr50(void);
void isr51(void);
void isr52(void);
void isr53(void);
void isr54(void);
void isr55(void);
void isr56(void);
void isr57(void);
void isr58(void);
void isr59(void);
void isr60(void);
void isr61(void);
void isr62(void);
void isr63(void);
void isr64(void);
void isr65(void);
void isr66(void);
void isr67(void);
void isr68(void);
void isr69(void);
void isr70(void);
void isr71(void);
void isr72(void);
void isr73(void);
void isr74(void);
void isr75(void);
void isr76(void);
void isr77(void);
void isr78(void);
void isr79(void);
void isr80(void);
void isr81(void);
void isr82(void);
void isr83(void);
void isr84(void);
void isr85(void);
void isr86(void);
void isr87(void);
void isr88(void);
void isr89(void);
void isr90(void);
void isr91(void);
void isr92(void);
void isr93(void);
void isr94(void);
void isr95(void);
void isr96(void);
void isr97(void);
void isr98(void);
void isr99(void);
void isr100(void);
void isr101(void);
void isr102(void);
void isr103(void);
void isr104(void);
void isr105(void);
void isr106(void);
void isr107(void);
void isr108(void);
void isr109(void);
void isr110(void);
void isr111(void);
void isr112(void);
void isr113(void);
void isr114(void);
void isr115(void);
void isr116(void);
void isr117(void);
void isr118(void);
void isr119(void);
void isr120(void);
void isr121(void);
void isr122(void);
void isr123(void);
void isr124(void);
void isr125(void);
void isr126(void);
void isr127(void);
void isr128(void);
void isr129(void);
void isr130(void);
void isr131(void);
void isr132(void);
void isr133(void);
void isr134(void);
void isr135(void);
void isr136(void);
void isr137(void);
void isr138(void);
void isr139(void);
void isr140(void);
void isr141(void);
void isr142(void);
void isr143(void);
void isr144(void);
void isr145(void);
void isr146(void);
void isr147(void);
void isr148(void);
void isr149(void);
void isr150(void);
void isr151(void);
void isr152(void);
void isr153(void);
void isr154(void);
void isr155(void);
void isr156(void);
void isr157(void);
void isr158(void);
void isr159(void);
void isr160(void);
void isr161(void);
void isr162(void);
void isr163(void);
void isr164(void);
void isr165(void);
void isr166(void);
void isr167(void);
void isr168(void);
void isr169(void);
void isr170(void);
void isr171(void);
void isr172(void);
void isr173(void);
void isr174(void);
void isr175(void);
void isr176(void);
void isr177(void);
void isr178(void);
void isr179(void);
void isr180(void);
void isr181(void);
void isr182(void);
void isr183(void);
void isr184(void);
void isr185(void);
void isr186(void);
void isr187(void);
void isr188(void);
void isr189(void);
void isr190(void);
void isr191(void);
void isr192(void);
void isr193(void);
void isr194(void);
void isr195(void);
void isr196(void);
void isr197(void);
void isr198(void);
void isr199(void);
void isr200(void);
void isr201(void);
void isr202(void);
void isr203(void);
void isr204(void);
void isr205(void);
void isr206(void);
void isr207(void);
void isr208(void);
void isr209(void);
void isr210(void);
void isr211(void);
void isr212(void);
void isr213(void);
void isr214(void);
void isr215(void);
void isr216(void);
void isr217(void);
void isr218(void);
void isr219(void);
void isr220(void);
void isr221(void);
void isr222(void);
void isr223(void);
void isr224(void);
void isr225(void);
void isr226(void);
void isr227(void);
void isr228(void);
void isr229(void);
void isr230(void);
void isr231(void);
void isr232(void);
void isr233(void);
void isr234(void);
void isr235(void);
void isr236(void);
void isr237(void);
void isr238(void);
void isr239(void);
void isr240(void);
void isr241(void);
void isr242(void);
void isr243(void);
void isr244(void);
void isr245(void);
void isr246(void);
void isr247(void);
void isr248(void);
void isr249(void);
void isr250(void);
void isr251(void);
void isr252(void);
void isr253(void);
void isr254(void);
void isr255(void);

/**
 * common interrupt handler. defined in isr_common.c.
 * this is called by each of the above stub functions
 * with the apropriate interrupt number.
 */
void isr_common(uint64_t int_no);

/* in isrtbl.c */
extern void *isrtbl[];

#endif
