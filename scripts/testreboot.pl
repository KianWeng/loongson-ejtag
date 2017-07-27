#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);
my ($error,$success,$total);
my $alarmset=0;
my $timeout=60;

sub reboot
{
outl(0xffffffffbfe0011c,0x603f0000);
}

sub check{
if($_[0])
{
	$error++;
}
else 
{
	$sucess++;
}

$total++;
alarm 0;
reboot();
alarm $timeout;
printf qq(\ntest %d, sucess %d ,error %d\n\n),$total,$sucess,$error;
printf F1 qq(\ntest %d, sucess %d ,error %d\n\n),$total,$sucess,$error;
};


$error=$success =$total = 0;

my $FILE=q(/dev/ttyUSB0);
system("date >> /tmp/test.log");
open F1,qq(>>/tmp/test.log);
open F,qq(<$FILE);
system qq( stty raw -echo  speed 115200  < $FILE);
local $SIG{ALRM} = sub { check(1); }; 
alarm $timeout;
reboot();
while(<F>)
{
print;
print F1 $_;

if(/rd_start=(\S+)\srd_size=(\S+)/)
{
$a= Math::BigInt->new("0xffffffff00000000")|hex($1);
printf("callbin bin/md5.bin %s $2;", $a->as_hex());
$ret = do_cmd1v("callbin bin/md5.bin %s $2;", $a->as_hex());
print $ret;
print F1 $ret;
if($ret !~ /f5fb3784f6609f56a5c1d4356ba12932/){
print "error now\n";
exit(0);
}
else
{
do_cmd("cont;");
}

}

if(/Please/)
{
check(0);
}
}
close F;
close F1;
