#/usr/bin/perl

sub usage{
die "uasage:$ARGV xres yres hsync cpu:from-to(M) dd:from-to(M) dc:from-to [ls1b01]" 
}

sub caclfreq1b
{
my @argv;
usage if @ARGV < 5;
@argv=@ARGV;

for($i=3;$i<=5;$i++)
{
 if($argv[$i]=~/(cpu|dc|ddr):([-\d.]+)-([\d.]+)$/)
 {
  $idx{$1} = $i-3;
  $area[$i-3][0] = $2;
  $area[$i-3][1] = $3;
  $area[$i-3][2] = 1;
 }
 else
 {
	usage;
 }
}


$exdiv=(@argv>5 && $argv[6])?1:4;

$area[$idx{dc}][2] = $exdiv;

open F,qq(gtf @argv[0..2]|);
while(<F>)
{
if(/Modeline\s+\S+\s+(\S+)/)
 {
	 $freq=$1;
	last;
 }
}
close F;

$area[$idx{dc}][0] += $freq;
$area[$idx{dc}][1] += $freq;


my $t=0;
	for($i=0;$i<=0x3f;$i++)
	{
		for($j=0;$j<=1023;$j++)
		{

				$gclk = 33.333*(12+$i+$j/1024)/2;
				next if($gclk>660);

				for($order=0;$order<=2;$order++)
				{	

					for($k=1;$k<=31;$k++)
					{
						 
						 $val=33.333*(12+$i+$j/1024)/2/$area[$order][2]/$k;

						if($val >= $area[$order][0] && $val <= $area[$order][1])
						{
						$k[$order]=$k;
						last;
						}
					}

			       last if($k==32);
				}

				next if($order < 3);

				$h[$t++] = [ $i,$j,@k ];
			
		}
	}





	if($idx{dc} == 0)
	{
	print qq(hah\n\n);
	@res = sort { abs(33.333*(12+$a->[0]+$a->[1]/1024)/2/$a->[2]/$area[$idx{dc}][2]-$freq) <=> abs(33.333*(12+$b->[0]+$b->[1]/1024)/2/$b->[2]/$area[$idx{dc}][2] - $freq) }  @h;
	}
	else
	{
	@res = sort { 33.333*(12+$b->[0]+$b->[1]/1024)/2/$b->[2] <=> 33.333*(12+$a->[0]+$a->[1]/1024)/2/$a->[2] }  @h;
	}
	printf "i,\tj,\tpll,\tcpu\tddr\tdcdiff\t8030,\t,8034\n";
	for($t=0;$t<@res;$t++)
   {
	($i,$j,$k[0],$k[1],$k[2])=@{$res[$t]};
	$gclk = 33.333*(12+$i+$j/1024)/2;
	$cpu = $gclk/$area[$idx{cpu}][2]/$k[$idx{cpu}];
	$ddr = $gclk/$area[$idx{ddr}][2]/$k[$idx{ddr}];
	$dc =  $gclk/$area[$idx{dc}][2]/$k[$idx{dc}] - $freq;

	$r8030=$i|($j<<8);
	$r8034=(1<<31)|($k[$idx{dc}]<<26)|(1<<25)|($k[$idx{cpu}]<<20)|(1<<19)|($k[$idx{ddr}]<<14);

	printf "%d:%d,\t%d,\t%f,\t%f,\t%f,\t%f,\t%x,\t,%x\n",$t,$i,$j,33.3333*(12+$i+$j/1024)/2 ,$cpu,$ddr,$dc,$r8030, $r8034;
   }
}

caclfreq1b;
