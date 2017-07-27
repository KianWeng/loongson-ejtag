#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);

my $cpu=hex(do_cmd1("setconfig core.cpuno"));
my $count=hex(do_cmd1("setconfig core.cpucount"));
do_cmd("savedm 1;jtagregs");
my @a;
for($i=0;$i<$count;$i++)
{
 do_cmd("setconfig core.cpuno $i");
 push @a,(inq(20)>>1);
 do_cmd("@ARGV") if(@ARGV);
}

map {print  $_->as_hex(),q( ); } @a;

do_cmd("savedm 0;");
do_cmd("setconfig core.cpuno $cpu");
print qq(\n);
