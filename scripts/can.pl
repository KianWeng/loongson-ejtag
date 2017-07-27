#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);

sub CAN0_BASE()
{
	return 0xbfe54000;
}

sub CAN1_BASE()
{
	return 0xbfe50000;
}


sub extern_testself_1()  
{
#reset mode
outb(CAN0_BASE,1);
outb(CAN1_BASE,1);
# externmode	
outb(CAN0_BASE+1,0x80);#gc
outb(CAN1_BASE+1,0x80);#gc
#set timing
outb(CAN0_BASE+6,3+(1<<6)); #gc3+(1<<6)   43
outb(CAN0_BASE+7,0x2f);  #gc2f  0xf+(0x2<<4)+(0<<7)
outb(CAN1_BASE+6,3+(1<<6)); #gc3+(1<<6)   43
outb(CAN1_BASE+7,0x2f);  #gc2f  0xf+(0x2<<4)+(0<<7)
outb(CAN0_BASE+5,0xff);  #gcff  mask == ff

outb(CAN0_BASE+16,0x0);
outb(CAN0_BASE+17,0x0);   #gc 这几个寄存器的功能？怎么设置.id帧
outb(CAN0_BASE+18,0x0);
outb(CAN0_BASE+19,0x0);
outb(CAN0_BASE+20,0xff);
outb(CAN0_BASE+21,0xff);
outb(CAN0_BASE+22,0xff);
outb(CAN0_BASE+23,0xff);

outb(CAN1_BASE+16,0x0);
outb(CAN1_BASE+17,0x0);   #gc 这几个寄存器的功能？怎么设置.id帧
outb(CAN1_BASE+18,0x0);
outb(CAN1_BASE+19,0x0);
outb(CAN1_BASE+20,0xff);
outb(CAN1_BASE+21,0xff);
outb(CAN1_BASE+22,0xff);
outb(CAN1_BASE+23,0xff);

outb(CAN0_BASE+4,0xff);  #gc 所有中断 使能
outb(CAN1_BASE+4,0xff);  #gc 所有中断 使能
#work mode
outb(CAN0_BASE,0x4); #gc04正常工作模式
outb(CAN1_BASE,0x4); #gc04正常工作模式
#set data
outb(CAN0_BASE+16,0x88);  #gcID 
outb(CAN0_BASE+17,0x12);
outb(CAN0_BASE+18,0x34);
outb(CAN0_BASE+19,0x45);
outb(CAN0_BASE+20,0x56);   #gc接收时，变0x50

outb(CAN0_BASE+21,0x1);
outb(CAN0_BASE+22,0x2);
outb(CAN0_BASE+23,0x3);
outb(CAN0_BASE+24,0x4);
outb(CAN0_BASE+25,0x5);
outb(CAN0_BASE+26,0x6);
outb(CAN0_BASE+27,0x7);
outb(CAN0_BASE+28,0x8);
#

#outb(CAN0_BASE+1,0x90);  #self request ok now
outb(CAN0_BASE+1,0x81);  #gctx_requeste
printf("wait..\n");
<STDIN>;
do_cmd("d4 0x%x 8\n",CAN1_BASE);
do_cmd("d4 0x%x 8\n",CAN0_BASE);
printf("\n");
outb(CAN1_BASE+1,0x84); #release buffer
outb(CAN0_BASE+1,0x84); #release buffer

}


#========================================== 扩展模式，can0 自己发，自己收. can1也一样, ok
# 能够接收数据 但是，outb(CAN0_BASE+20,0x56); 收到0x50.不是0x56

sub extern_testself_2()
{
#reset mode
outb(CAN0_BASE,1);
outb(CAN1_BASE,1);
#outb(CAN0_BASE+31,0x80);
# externmode
outb(CAN0_BASE+1,0x90);#gc自接收请求

#set timing
outb(CAN0_BASE+6,3+(1<<6)); #gc3+(1<<6)   43
outb(CAN0_BASE+7,0x2f);  #gc2f  0xf+(0x2<<4)+(0<<7)
outb(CAN0_BASE+5,0xff);  #gcff  mask == ff

outb(CAN0_BASE+16,0x0);
outb(CAN0_BASE+17,0x0);   #gc 这几个寄存器的功能？怎么设置.id帧
outb(CAN0_BASE+18,0x0);
outb(CAN0_BASE+19,0x0);
outb(CAN0_BASE+20,0xff);
outb(CAN0_BASE+21,0xff);
outb(CAN0_BASE+22,0xff);
outb(CAN0_BASE+23,0xff);

outb(CAN0_BASE+4,0xff);  #gc 所有中断 使能
#work mode
outb(CAN0_BASE,0x04); #gc04正常工作模式
#set data
outb(CAN0_BASE+16,0x88);  #gcID 
outb(CAN0_BASE+17,0x12);
outb(CAN0_BASE+18,0x34);
outb(CAN0_BASE+19,0x45);
outb(CAN0_BASE+20,0x56);

outb(CAN0_BASE+21,0x1);
outb(CAN0_BASE+22,0x2);
outb(CAN0_BASE+23,0x3);
outb(CAN0_BASE+24,0x4);
outb(CAN0_BASE+25,0x5);
outb(CAN0_BASE+26,0x6);
outb(CAN0_BASE+27,0x7);
outb(CAN0_BASE+28,0x8);
#release buffer
outb(CAN0_BASE+1,0x94);
outb(CAN0_BASE+1,0x90);  #gctx_requeste
printf("wait..\n");
<STDIN>;
do_cmd("d4 0x%x 8\n",CAN1_BASE);
printf("\n");
}

sub hello2()  #gc  2个脚can0_h--can1_h,can0_l--can1_l链接,
		 #gc  标准模式:can0_tx and can1_rx.  ok 
{
	
#reset mode
outb(CAN1_BASE,1);
#leave externmode
outb(CAN1_BASE+1,0);
#set timing
outb(CAN1_BASE+6,3+(1<<6)); #gc3+(1<<6)  0x43
outb(CAN1_BASE+7,0xf+(0x2<<4)+(0<<7));  #gc 0x2f

outb(CAN1_BASE+0x4,0xa5); #gc== outb(CAN0_BASE+10,0xa5)
outb(CAN1_BASE+0x5,0xff);
#work mode
outb(CAN1_BASE,0);


#reset mode
outb(CAN0_BASE,1);
#leave externmode
outb(CAN0_BASE+1,0);
#set timing
outb(CAN0_BASE+6,3+(1<<6)); #gc3+(1<<6)    0x43
outb(CAN0_BASE+7,0xf+(0x2<<4)+(0<<7)); #gc 0x2f

outb(CAN0_BASE+0x4,0xa5); #gc acr  == outb(CAN0_BASE+10,0xa5)
outb(CAN0_BASE+0x5,0xff); #gc amr  mask
#work mode
outb(CAN0_BASE,0);
#set data
outb(CAN0_BASE+10,0xa5);
outb(CAN0_BASE+11,0x28);
outb(CAN0_BASE+12,0x1);
outb(CAN0_BASE+13,0x2);
outb(CAN0_BASE+14,0x3);
outb(CAN0_BASE+15,0x4);
outb(CAN0_BASE+16,0x5);
outb(CAN0_BASE+17,0x6);
outb(CAN0_BASE+18,0x7);
outb(CAN0_BASE+19,0x8);
printf("press to send\n");
<STDIN>;
outb(CAN0_BASE+1,0x1);  #gctx_requeste


<STDIN>;
do_cmd("d4 0x%x 8\n",CAN0_BASE);
do_cmd("d4 0x%x 8\n",CAN1_BASE);
printf("\n");
#release buffer
outb(CAN1_BASE+1,0x4); #release buffer

}

sub sw_init{
outb($_[0]+0,0x1);
outb($_[0]+1,0x80);
outb($_[0]+6,0x43);
outb($_[0]+7,0x45);
}

sub sw_desc_init{
outb($_[0]+16,0);
outb($_[0]+17,0);
outb($_[0]+18,0);
outb($_[0]+19,0);
outb($_[0]+20,0xff);
outb($_[0]+21,0xff);
outb($_[0]+22,0xff);
outb($_[0]+23,0xff);
}

sub sw_set_mode{
outb($_[0],4);
outb($_[0]+4,0xff);
}

sub sw_send_frame{
my @d=(0x83,0x12,0x74,0x45,0x56,0xde,0xad,0xbe);
my $i=16;
for (@d)
{
outb($_[0]+$i++,$_);
}
#outb($_[0]+1,0x90); #self test 
outb($_[0]+1,0x81);
}


sub sw_can{
	sw_init(CAN0_BASE);
	sw_init(CAN1_BASE);
	sw_desc_init(CAN0_BASE);
	sw_desc_init(CAN1_BASE);
	sw_set_mode(CAN0_BASE);
	sw_set_mode(CAN1_BASE);
	outb(CAN1_BASE+1,0x0); # only listen
	sw_send_frame(CAN0_BASE);
	print qq(wait..\n);
	<STDIN>;
	do_cmd("d4 0x%x 8\n",CAN1_BASE);
	do_cmd("d4 0x%x 8\n",CAN0_BASE);
}

do_cmd("echo_on");
#sw_can;
#hello2;
extern_testself_1
#extern_testself_2

