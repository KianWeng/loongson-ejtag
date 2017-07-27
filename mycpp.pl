#!/usr/bin/perl
use File::Temp;
while(<>)
{
if(/(.*)tgt_compile\s*\(\s*(".*")\s*\)(.*)/)
{
$head=$1;
$self=$2;
$tail=$3;
$tmpname=File::Temp::tempnam( "/tmp", "mycpp_");

print qq(${head}\{);
	open F,">$tmpname.S";
	print F ".set mips64;\n.set noreorder;\n.set noat;\n";
	while($self=m/"(([^"]|\\")*[^\\])"/g)
	{
	($a=$1)=~s/deret/.word 0x4200001f;\n/g;
	$a=~s/\\"/"/g;
	$a=~s/\\n/\n/g;
	$a=~s/move(\s.+)([;]\s+)$/or\1,\$0\2/g;
	print F "$a";
	}
	close F;
	if(index(`uname -m`,"mips")<0)
	{
	$ENV{CROSS_COMPILE}=q(mipsel-linux-) unless defined($ENV{CROSS_COMPILE});
	}
	system("\${CROSS_COMPILE}gcc -include include/regdef.h -c -o $tmpname.o $tmpname.S && \${CROSS_COMPILE}objcopy  -j .text -O binary $tmpname.o $tmpname.bin");
	if($?)
	{
	print qq(#error mycpp $ENV{CROSS_COMPILE}gcc -include include/regdef.h -c -o $tmpname.o $tmpname.S\n);
	}
	else
	{
	my @a =`\${CROSS_COMPILE}objdump  -j .text -m mips:isa64 -M no-aliases -d $tmpname.o`;
	my @b= `\${CROSS_COMPILE}objdump  -j .text -m mips:isa32r2 -M no-aliases -d $tmpname.o`;
	my $i;
	for($i=0;$i<@a;$i++)
	{
		chomp ($a[$i],$b[$i]);
		last if($a[$i] ne $b[$i] && $a[$i]!~/(sd|ld|daddiu|daddu|dsubu|dsrl|dmfc0|dmtc0|dmfc1|dtmc1)/);
	}

	if($i!=@a)
	{
	print qq(#warning ins mips64: $a[$i]  mips32: $b[$i]\n);
	}
		unlink qq($tmpname.o);
		unlink qq($tmpname.S);
	open FS,"$tmpname.bin";
	my ($x,$i,$d,$code);
	$i=0;
	while(!eof(FS))
	{
	$code = $x;
	printf  ("/*%08x:*/",$i*4) if($i%4==0);
	read FS,$d,4;
	$x=unpack "I",$d;
	printf  ("0x%08x",$x);
	$i++;
	if(!eof(FS))
	{
	printf  (","); 
	}
	elsif($i>1 && ($code&0xffff0000)==0x10000000)
	{
#check b 1b, nop at the end, if so and a extra nop
	printf  (", /*fix branch*/0"); 
	}
	}
	close(FS);
		unlink qq($tmpname.bin);
	}

print qq(}$tail\n);

}
else { print; }
}
