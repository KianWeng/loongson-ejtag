#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);

sub ffs{
my $i;
my $num=int($_[0]);
for($i=31;$i>=0;$i--)
{
 last if($num&(1<<$i));
}
return $i;
}

sub config{
do_cmd("savedm 1;");
do_cmd("cp0s 0");
my $prid = inl(15);
if($prid == 0x6305)
{
do_cmd("cp0s 2");
my $config=inl(16);
printf qq(scache line %d\n),($slines=2<<(($config>>4)&15));
printf qq(scache sets %d\n),($ssets=64<<(($config>>8)&15));
printf qq(scache ways %d\n),($sways=1+(($config)&15));

my $cpucount = hex(do_cmd1("setconfig core.cpucount"));
printf qq(scache size %d\n),($ssize=$ssets*$slines*$sways*$cpucount);
$swaybit=0;
$iwaybit=0;
$dwaybit=0;
do_cmd(<<AAA
setconfig scache.size $ssize
setconfig scache.ways $sways
setconfig scache.waybit $swaybit
AAA
);
}

do_cmd("cp0s 1");
my $config=inl(16);
printf qq(icache sets %d\n),($isets=64<<(($config>>22)&7));
printf qq(icache line %d\n),($ilines=2<<(($config>>19)&7));
printf qq(icache ways %d\n),($iways=1+(($config>>16)&7));
printf qq(icache size %d\n),($isize=$isets*$ilines*$iways);

if($prid != 0x6305)
{
$iwaybit=ffs($isets*$ilines);
}


printf qq(dcache sets %d\n),($dsets=64<<(($config>>13)&7));
printf qq(dcache line %d\n),($dlines=2<<(($config>>10)&7));
printf qq(dcache ways %d\n),($dways=1+(($config>>7)&7));
printf qq(dcache size %d\n),($dsize=$dsets*$dlines*$dways);

if($prid != 0x6305)
{
$dwaybit=ffs($dsets*$dlines);
}

do_cmd(<<AAA
setconfig icache.size $isize
setconfig icache.ways $iways
setconfig icache.waybit $iwaybit
setconfig dcache.size $dsize
setconfig dcache.ways $dways
setconfig dcache.waybit $dwaybit
AAA
);
do_cmd("savedm 0;");
}

sub init2{
do_cmds(<<AAA
cp0s
m4 29 0
m4 28 0
m4 26 0x22
mems

cache 9 0xffffffff800000000 0x4000
cache 9 0xffffffff800000001 0x4000
cache 9 0xffffffff800000002 0x4000
cache 9 0xffffffff800000003 0x4000

cp0s
m4 29 0
m4 28 0
m4 26 0
mems

cache 8 0xffffffff800000000 0x4000
cache 8 0xffffffff800000001 0x4000
cache 8 0xffffffff800000002 0x4000
cache 8 0xffffffff800000003 0x4000

cp0s
m4 29 0
m4 28 0
m4 26 0x22
mems

cache 0xb 0xffffffff800000000 0x4000
cache 0xb 0xffffffff800000001 0x4000
cache 0xb 0xffffffff800000002 0x4000
cache 0xb 0xffffffff800000003 0x4000

cp0s
m4 16 3
mems
AAA
);
}

sub init{
do_cmds(<<AAA
cp0s
m4 29 0
m4 28 0
mems

cache 9 0xffffffff9fc00000 0x4000
cache 0 0xffffffff9fc00000 0x4000
cache 1 0xffffffff9fc00000 0x4000

cp0s
m4 16 3
mems
AAA
);
}

sub cachelock2{
do_cmds(<<AAA
m8 0xffffffffbfd84210  0x800000001fc00000
m8 0xffffffffbfd84250  0xfffffffffffc0000
cache 0x13 0xffffffff9fc00000 0x4000
AAA
);
}

sub cachelock3{
do_cmds(<<AAA
m8 0x900000003ff00200  0x800000001fc00000
m8 0x900000003ff00240  0xfffffffffffc0000
#cache 0x13 0xffffffff9fc00000 0x4000
AAA
);
}


open(STDIN,q(</dev/null));

eval "@ARGV";
