#!/usr/bin/perl

$ENV{LC_ALL} = "C";

 my $f=q(/work/ls1g+/pmon/Targets/LS1F/compile/ls1gp/pmon);
 open F, qq(readelf -W -s /work/ls1g+/pmon/Targets/LS1F/compile/ls1gp/pmon|);
 while(<F>)
 {
  my @a = split /\s+/, $_;
  {
   next if ! defined($a[8]);
  $sym{$a[8]}= [ hex($a[2]), $a[3], $a[8], 0 ];
  for($i=hex($a[2]);$i<hex($a[2])+$a[3];$i+=4)
  {
  $func{$i} = $sym{$a[8]};
  }
  }
 }
  close F;

open F,q(1.dat);
my $ret;
my $cnt;
my $pc=0x80010000;

do
{
	$ret = read F,$buf,4;
	if($ret == 4)
	{
		$cnt = unpack(q(I),$buf);
		if($cnt)
		{
			$pcs{$pc} = $cnt;
			$func{$pc}->[3] += $cnt;
		}
	}
	$pc = $pc+4;
} while($ret == 4);
close F;

my @pcorder = sort {$pcs{$b} <=> $pcs{$a} } keys %pcs;

printf("-----------pc--------------\n");

for( @pcorder)
{
printf "%x %x %s+0x%x\n", $_,$pcs{$_}, $func{$_}->[2], $_-$func{$_}->[0];
}

printf("-----------func--------------\n");

my @funcorder = sort {$sym{$b}->[3] <=> $sym{$a}->[3] } keys %sym;
for( @funcorder)
{
last if !$sym{$_}->[3];
printf "%s %x\n", $_,$sym{$_}->[3];
}

