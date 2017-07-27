#!/usr/bin/perl

$ENV{LC_ALL} = "C";

open F,q(./linux-wrs/arch/mips/huaya/drivers/regdma/regdma_r2/LK_DRV_REGDMA_R2.ko);
{
local $/;
$a=<F>;
$t=pack(q(I*),map{hex($_)} @ARGV);
$off = index($a,$t,0);
}

open F,q(readelf -e -W ./linux-wrs/arch/mips/huaya/drivers/regdma/regdma_r2/LK_DRV_REGDMA_R2.ko|);
while(<F>)
{
   if(/Section Headers:/)
   {
    <F>;
    <F>;
    last;
    }
}


while(<F>)
{
    last if(/Key to Flags:/);
   @a= split /\s+/,substr($_,7,1000);
   $addr = hex($a[3]);
   $sect{$addr}=$a[0];
}

@res = sort {$a <=> $b}  keys %sect;


for($i=0;$i<@res;$i++)
{
    last if($off<$res[$i]);
}

printf "%s+0x%x\n", $sect{$res[$i-1]}, $off-$res[$i-1];
