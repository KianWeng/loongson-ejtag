#!/usr/bin/perl
use IO::Socket;

$ENV{EJTAGEXE}=qq(./ejtag_debug_pp) if !defined($ENV{EJTAGEXE});
$abisize=`$ENV{EJTAGEXE} setconfig core.abisize`;
chomp $abisize;
$ibc = (hex(`$ENV{EJTAGEXE} d4q 0xffffffffff301000 1`)>>24)&0xf;
$dbc = (hex(`$ENV{EJTAGEXE} d4q 0xffffffffff302000 1`)>>24)&0xf;
$port=50010;

for($port=50010;;$port++)
{
$sock=IO::Socket::INET->new(LocalPort=>$port);
last if($sock)
}
close $sock;

if(fork())
{
open F,qq(>null);
close F;
system qq($ENV{EJTAGEXE} gdbserver $port 1 < null\n);
unlink qq(null);
wait;
}
else
{
sleep 5;

open F,q(>gdb.cmd);
if ($abisize eq "00000040")
{
print F  <<AAA
set architecture mips:isa64r2
set mips abi n64
set remotetimeout 20
AAA
;
}
else
{
print F  <<AAA
set architecture mips:isa32r2
set mips abi o32
set remotetimeout 20
AAA
;
}

print F <<AAA
set remote hardware-breakpoint-limit $ibc
set remote hardware-watchpoint-limit $dbc
define mysi
monitor si 0
c
end
AAA
;

print F  "target remote 127.0.0.1:$port\n";
close F;
if(!$ENV{gdbserver})
{
system qq(gdb-multiarch -q -x gdb.cmd @ARGV);
}
exit 0;
}

