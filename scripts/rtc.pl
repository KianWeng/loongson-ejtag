#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);
local $BASE = 0xffffffffbfe64000;

sub printrtc{
my $t=$_[0];
printf("%d:%d:%d.%d\n",($t>>16)&0x1f,($t>>10)&0x3f,($t>>4)&0x3f,$t&0xf);
}
sub readrtc{
outl($BASE+0x40,0x2d00);
$t=inl($BASE+0x2c);
printrtc($t);
}

sub setrtc{
@a=split /:/,$_[0];
$t=($a[0]<<16)|($a[1]<<10)|($a[2]<<4);
outl($BASE+0x28,0);
outl($BASE+0x24,$t);
}

sub testalarm{
outl($BASE+0x28,0);
outl($BASE+0x24,0);
outl(0xbfd01054,0x1000000);
outl(0xbfd01044,0x1000000);
outl(0xbfd0104c,0x1000000);
outl($BASE+0x40,0x2d00);
my $t=inl($BASE+0x2c);
my $t1;
#alarm in 5s
$t1=($t&~0x3f0)|((($t&0x3f0)+(5<<4))&0x3f0);
outl($BASE+0x34,($t1>>4));
printrtc $t;
printrtc $t1;
printf "%x\n",inl(0xbfd01040);
sleep 10;
printf "%x\n",inl(0xbfd01040);
}

if(@ARGV>0)
{
#setrtc $ARGV[0]; 
setrtc `date +%H:%M:%S`;
}
else
{
readrtc;
}

