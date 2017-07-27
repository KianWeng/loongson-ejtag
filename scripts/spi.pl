#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);

sub test_cs{
do_cmd("spi_init");
for(my $i=4;$i<8;$i++)
{
outb(0xbfe80005,(1<<$i)|0xf);
<STDIN>;
}
}

sub SPCR(){return 0;}
sub SPSR(){return 1;}
sub TXFIFO(){return 2;}
sub SPER(){return 3;}
sub PARAM(){return 4;}
sub SOFTCS(){return 5;}
sub PARAM2(){return 6;}

sub spi_init{
outb($_[0]+SPCR,0);
outb($_[0]+SPSR,0xc0);
outb($_[0]+PARAM,0x10);
outb($_[0]+SPER,0x5);
outb($_[0]+PARAM2,0x1);
outb($_[0]+SPCR,0x50);
}

do_cmd("echo_on");

spi_init(0xbfe80000); 
outb(0xbfe80002,0x55);
