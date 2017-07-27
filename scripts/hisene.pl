#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);

$ENV{LC_ALL} = "C";

my $ddrparam = <<'AAA'
mml 0xbf200200 0x0002aaaa
mml 0xbf200204 0x05555555
mml 0xbf200208 0x02a80000
mml 0xbf20020c 0x0000aa55
mml 0xbf200210 0x05555555
mml 0xbf2000fc 0x00006c00
mml 0xbf204000 0x04737220
mml 0xbf204004 0x93020012
mml 0xbf204008 0x1b131327
mml 0xbf20400c 0x24373f23
mml 0xbf204010 0x00000008
mml 0xbf204014 0x82000000
mml 0xbf204060 0x02a063f3
mml 0xbf204064 0x0e200000
REM delay(12000)
mml 0xbf20409c 0x00e21809
mml 0xbf2040a0 0x04800000
mml 0xbf20401c 0x00e7100a
mml 0xbf204020 0x00200093
mml 0xbf204024 0x5a70afd2
mml 0xbf204028 0xfd000000
mml 0xbf20402c 0x350d2510
mml 0xbf204030 0x350d2510
mml 0xbf204034 0x11053f26
mml 0xbf204040 0x02e14802
mml 0xbf204044 0x80020000
REM delay(12000)
mml 0xbf2000c4 0x03100000
mml 0xbf204050 0x8008801f
mml 0xbf204054 0x7792a000
mml 0xbf204058 0xa01ff742
mml 0xbf20405c 0xa0000000
REM delay(400000)
mml 0xbf2000fc 0x80006c00
mml 0xbf200090 0x001000c8
mml 0xbf2000fc 0xc0006c00
REM delay(12000)
mml 0xbf200094 0x000010c6
mml 0xbf200098 0x00000000
mml 0xbf20009c 0x0000003d
mml 0xbf2000a0 0x0000003f
mml 0xbf2000a4 0x000001ef
mml 0xbf2000ac 0x60000003
mml 0xbf2000b0 0x00000000
mml 0xbf2000b4 0x8fffffff
mml 0xbf2000bc 0x00801002
mml 0xbf2000c4 0x033fffff
REM delay(12000)
mml 0xbf200080 0xfc3caf6c
mml 0xbf200088 0xffffffff
REM delay(12000)
mml 0xbf200220 0x1513131b
mml 0xbf200224 0x15000019
mml 0xbf200228 0x19151515
mml 0xbf20022c 0x10101010
mml 0xbf200230 0x15001515
mml 0xbf200234 0x15151515
mml 0xbf200238 0x15151010
mml 0xbf20023c 0x10001000
mml 0xbf200240 0x10101000
mml 0xbf200244 0x00000000
mml 0xbf200248 0x15151315
mml 0xbf20024c 0x151a1515
mml 0xbf200250 0x15151515
mml 0xbf200254 0x15151a19
mml 0xbf200258 0x1a151515
mml 0xbf20025c 0x1a1a1515
REM delay(12000)
mml 0xbf2200c0 0x00000190
mml 0xbf2200c4 0x000000f8
mml 0xbf2200cc 0x00000028
mml 0xbf2200c8 0x000002f4
mml 0xbf220080 0x00040021
mml 0xbf230018 0x0001c000
mml 0xbf230044 0x0000040b
mml 0xbf230054 0x00000a20
mml 0xbf230058 0x00000000
mml 0xbf23005c 0x00000000
mml 0xbf230060 0x00000000
mml 0xbf230048 0x81188855
mml 0xbf23004c 0x1a825280
mml 0xbf230050 0x90021050
mml 0xbf23000c 0x0300c407
mml 0xbf2202c4 0x00000002
mml 0xbf230068 0x010015cf
mml 0xbf230004 0x08000000
mml 0xbf230004 0x000001ff
REM dmc_polling(0xbf230010,0x8000001f)
REM dmc_polling(0xbf2202c0,0x00000011)
mml 0xbf220044 0x00000001
REM dmc_polling(0xbf220048,0x00000001)
mml 0xbf2200c0 0x00000190
mml 0xbf2200c4 0x000000f8
mml 0xbf2200c8 0x000002f4
mml 0xbf2200cc 0x00000028
mml 0xbf2200d0 0x00000040
mml 0xbf2200d4 0x00000004
mml 0xbf2200d8 0x0000004a
mml 0xbf2200dc 0x00000008
mml 0xbf2200e0 0x00000002
mml 0xbf2200e4 0x00000000
mml 0xbf2200e8 0x00000006
mml 0xbf2200ec 0x00000005
mml 0xbf2200f0 0x00000018
mml 0xbf2200f4 0x00000020
mml 0xbf2200f8 0x00000008
mml 0xbf2200fc 0x00000004
mml 0xbf220100 0x00000005
mml 0xbf220104 0x0000000a
mml 0xbf220108 0x00000005
mml 0xbf22010c 0x00000200
mml 0xbf220110 0x00000004
mml 0xbf220114 0x00000010
mml 0xbf220118 0x00000040
mml 0xbf22011c 0x00000000
mml 0xbf220124 0x00000007
mml 0xbf220128 0x00000007
mml 0xbf22012c 0x00000004
mml 0xbf220130 0x0000000c
mml 0xbf220134 0x00000043
mml 0xbf220138 0x00000200
mml 0xbf220140 0x00000005
mml 0xbf220144 0x00000000
mml 0xbf220254 0x00000001
mml 0xbf220260 0x00000001
mml 0xbf220484 0x00000016
mml 0xbf220000 0x00000300
mml 0xbf220040 0x88100000
REM dmc_polling(0xbf220040,0x08100000)
mml 0xbf220040 0x80140003
REM dmc_polling(0xbf220040,0x00140003)
mml 0xbf220040 0x80160003
REM dmc_polling(0xbf220040,0x00160003)
mml 0xbf220040 0x80120003
REM dmc_polling(0xbf220040,0x00120003)
mml 0xbf220040 0x8010b203
REM dmc_polling(0xbf220040,0x0010b203)
mml 0xbf220040 0x80100005
REM dmc_polling(0xbf220040,0x00100005)
mml 0xbf220004 0x00000001
REM dmc_polling(0xbf220008,0x00000001)
mml 0xbf220050 0x00000001
REM dmc_polling(0xbf22004c,0x00000001)
mml 0xbf230004 0x0000ffff
REM dmc_polling(0xbf230010,0x80000fff)
mml 0xbf220004 0x00000002
REM dmc_polling(0xbf220008,0x00000003)
mml 0xbf200054 0x00001ffe
mml 0xbf200050 0x0000012f
AAA
;

sub REM_delay{
select(undef, undef, undef, $_[0]/1000000);
}

sub REM_dmc_polling{
while(1)
{
$v=inl($_[0]);
$w=$_[1];
printf("%x,%x,val=0x%x,w=0x%x\n",$_[0],$_[1],$v,$w);
last if($v==$w);
}
}

sub md5sum_mem{
my $sum;
($sum=do_cmd1v(q(callbin bin/md5.bin 0x%x 0x%x),$_[0],$_[1]))=~s/.*md5sum is (\w+).*$/\1/m;
return $sum;
}


sub md5sum_file{
($sum=`dd if=/work/hisense/lwl/linux-2.6.27/vmlinux bs=1 skip=$_[0] count=$_[1] 2>/dev/null|md5sum -`)=~s/\s.+$//g;
return $sum;
}


sub md5sum_mem_segment
{
my (@F,$sum);
@F=qx(readelf -l /work/hisense/lwl/linux-2.6.27/vmlinux|grep LOAD|tail -n +$_[0]|head -n 1)=~/[\da-f]{6,}/g;
printf("md5sum 0x%x 0x%x\n",hex($F[1]),hex($F[1]));
$sum = md5sum_mem(hex($F[1]),hex($F[3]));
return $sum;
}


sub md5sum_file_segment{
my (@F,$sum);
@F=qx(readelf -l /work/hisense/lwl/linux-2.6.27/vmlinux|grep LOAD|tail -n +$_[0]|head -n 1)=~/[\da-f]{6,}/g;
($sum=`dd if=/work/hisense/lwl/linux-2.6.27/vmlinux bs=1 skip=@{[hex($F[0])]} count=@{[hex($F[3])]} 2>/dev/null|md5sum -`)=~s/\s.+$//g;
return $sum;
}

sub md5sum_mem_section{
my (@F,$sum);
@F=qx(readelf -S  /work/hisense/lwl/linux-2.6.27/vmlinux|grep " $_[0]" )=~/[\da-f]{6,}/g;
$sum = md5sum_mem(hex($F[0]),hex($F[2]));
return $sum;
}

sub md5sum_file_section{
my (@F,$sum);
@F=qx(readelf -S  /work/hisense/lwl/linux-2.6.27/vmlinux|grep " $_[0]" )=~/[\da-f]{6,}/g;
($sum=`dd if=/work/hisense/lwl/linux-2.6.27/vmlinux bs=1 skip=@{[hex($F[1])]} count=@{[hex($F[2])]} 2>/dev/null|md5sum -`)=~s/\s.+$//g;
return $sum;
}

sub ddr3_config{
my $set = defined($_[0])?0:1;
while(1)
{
if(!$set || inl(0xbf200050) != 0x0000012f)
{
do_cmd("d4 0xbfc00000 100");
open F,qq(>/tmp/ddr.S);
while($ddrparam=~/(.+)\n/mg)
{
$line=$1;
if($line=~/mml (\w+) (\w+)/)
{
print F qq(li t0,$1\nli t1,$2\nsw t1,0(t0)\nDELAY1(100000)\n\n);
 outl(hex($1),hex($2)) if $set;
}
elsif($line =~/REM delay\((\d+)\)/)
{
print F qq(//REM bal delay($1)\nDELAY1(1000000)\nnop\n\n);
 REM_delay($1) if $set;
}
elsif($line=~/REM dmc_polling\((\w+),(\w+)\)/)
{
print F qq(//REM_dmc_polling(hex($1),hex($2))\nDELAY1(100000)\nnop\n\n);
 REM_dmc_polling(hex($1),hex($2)) if $set;
}
}
close F;

do_cmds(<<AAA
echo_on
setconfig putelf.uncached 1
cp0s
m4 29 0
m4 28 0
mems

cache 9 0x80000000 0x4000
cache 0 0x80000000 0x4000
cache 1 0x80000000 0x4000

cp0s
m4 16 3
mems
AAA
);

}

if(inl(0xa0000004) != 0)
{
exit -1;
}

print qq(ddr init error\n");
}
}


sub all{
#outl(0xbf204004,0x93020019);

ddr3_config;

$f=do_cmd2(qq( d4 0xa0000000 10;memtest 0xa0000400 0xa0000600));
while(<$f>)
{
print;
if(/error/){close $f;exit -1;}
}

while(1)
{
do_cmds(<<AAA
echo_on
putelf /work/hisense/pmon4hisense/zloader/gzrom
cont
AAA
);
sleep 5;
$pc=hex(do_cmd1("cpuregs d4q 37 1"));
printf("pc=0x%x\n",$pc);
last if($pc != 0xbfc00380);
}

my $fsize = -s q(/work/hisense/lwl/linux-2.6.27/vmlinux);

($sum=`md5sum /work/hisense/lwl/linux-2.6.27/vmlinux`)=~s/\s.+$//g;


do_cmds(<<AAA
setconfig putelf.uncached 1
putelf /work/hisense/lwl/linux-2.6.27/vmlinux
AAA
);

$sum=md5sum_file_section(".text");
$sum1=md5sum_mem_section(".text");

if($sum != $sum1)
{
print qq(sum error);
exit -1;
}

$sum=md5sum_file_section(".init.ramfs");
$sum1=md5sum_mem_section(".init.ramfs");

if($sum != $sum1)
{
print qq(sum error);
exit -1;
}

$sum=md5sum_file_section(".init.text");
$sum1=md5sum_mem_section(".init.text");

if($sum != $sum1)
{
print qq(sum error);
exit -1;
}

do_cmds(<<AAA
karg console=ttyS0,115200 rdinit=/sbin/init initcall_debug=1
cont
AAA
);

}
if(@ARGV)
{
eval "@ARGV";
}
else
{
all;
}
