#define MYDBG li t2,__LINE__;sw t2,HEX_ADDR(s1);
.set mips32
.set noreorder
.set noat
li s1,0xff200000;
#spi io base
li s0,0xbfe80000;
li v0,0x11; /*high cs*/
sb v0,5(s0);
bal 112f; /*wait_sr(v0)*/
nop;
li v0,0x1;
sb v0,5(s0);
li v0,6;
bal 103f;
nop;
li v0,0x11;
sb v0,5(s0);
li v0,0x1;
sb v0,5(s0);
li t0,0x2;
300:
li v0,0x05;
bal 103f;
nop;
andi v0,0x3;
bne v0,t0,300b;
nop;
li v0,0x11;
sb v0,5(s0);
li v0,0x1;
sb v0,5(s0);
li v0,0x1;
bal 103f;
nop;
li v0,0x0;
bal 103f;
nop;
li v0,0x11;
sb v0,5(s0);
1:
lw s2,(a0)
12:
bal 112f;nop;
li v0,0x1;
sb v0,5(s0);
li v0,6;
bal 103f;
nop;
li v0,0x11;
sb v0,5(s0);
li v0,0x1;
sb v0,5(s0);
li t0,0x2;
300:li v0,0x05;
bal 103f;
nop;
andi v0,0x3;
bne v0,t0,300b;
nop;
li v0,0x11;
sb v0,5(s0);
li v0,0x1;
sb v0,5(s0);
li v0,2;
bal 103f;
nop;
and v0,a1,0xfff;
bnez v0,11f;
nop;
sw a1,HEX_ADDR(s1);
11:srl v0,a1,16;
bal 103f;
nop;
srl v0,a1,8;
bal 103f;
nop;
move v0,a1;
bal 103f;
nop;
2:
andi v0,s2,0xff
bal 103f;
nop;
srl s2, 8

li v0,0x11;
sb v0,5(s0);
addiu a0,1;
addiu a1,1;
addiu a2,-1;
beqz a2,3f;
nop;
andi v0, a0,3
bnez v0,12b
nop
b 1b;
nop;
3:li v0,0x11;
sb v0,5(s0); /*high cs*/
bal 112f;
nop;
sw a2,RET_ADDR(s1);
sync;
li ra,0xff200200 ;
jr ra ;
nop;
103:
sb v0,2(s0);
1:lb v0,1(s0);
andi v0,1;
bnez v0,1b;
nop;
lb v0,2(s0);
jr ra;
nop;
112:
move t0,ra;
li v0,0x1;
sb v0,5(s0);
li v0,0x5;
bal 103b;
nop;
1120:
li  v0,0xff;
bal  103b;
nop;
#sb v0,HEX_ADDR(s1)
andi v0,1;
bnez v0,1120b;
nop;
li v0,0x11;
sb v0,5(s0);
move ra,t0
jr  ra;
nop;
