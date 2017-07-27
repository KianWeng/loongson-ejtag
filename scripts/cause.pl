#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);
my $causereg=hex(do_cmd1(q(cp0s 0 d4q 13 1)));

my $cause=($causereg&0x7c)>>2;
@causestr=("int","tlbm","tlbl","tlbs","adel","ades","ibe","dbe","sys","bp","ri","cpu","ov","tr");
$causestr[15] = "fpe";

printf "cause is " . $causestr[$cause] . "\n";

if($cause == 8 || $cause == 0) { do_cmd("cont;"); }

