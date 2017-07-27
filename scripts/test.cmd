function gdb
setconfig gdbserver.sim 1
letl pid $(fork)
do if $pid
gdbserver 50010 1
wait
else
> gdb.cmd echo_n
 do if $(expr $(setconfig core.abisize)==32)
>> gdb.cmd echo set architecture mips:isa32r2
>> gdb.cmd echo set mips abi o32
 else
>> gdb.cmd echo set architecture mips:isa64r2
>> gdb.cmd echo set mips abi n64
 end
>> gdb.cmd echo set remotetimeout 20
>> gdb.cmd echo set remote hardware-breakpoint-limit 0
>> gdb.cmd echo set remote hardware-watchpoint-limit 0
>> gdb.cmd echo define mysi
>> gdb.cmd echo monitor si 0
>> gdb.cmd echo c
>> gdb.cmd echo end
>> gdb.cmd echo target remote 127.0.0.1:50010
shell ./mipsel-gdb -x gdb.cmd
exit 0
end
ret

function sum
local a i
let a i 0 0
do while $(expr $i<=$1)
let a $(expr $a+$i)
let i $(expr $i+1)
end
echo $a
ret $a

function sum1
for letl a i 0 0;$(expr $i<=$1);letl i $(expr $i+1)
let a $(expr $a+$i)
end
echo $a
ret $a


function fib
local i r
let i $1
do if $(expr $i==0)
let r 0
elsif $(expr $i==1)
 let r 1
else
 let r $(expr $(call fib $(expr $i-1))+$(call fib $(expr $i-2)))
end
echo $r
ret $r

function fib1
local i n r0 r1
letl i r0 r $1 0 1

do if $(expr $i==0)
let r 0
elsif $(expr $i==1)
 let r 1
else
 for letl n 2;$(expr $n<=$i);letl n $(expr $n+1)
 letl r0 r $r $(expr $r+$r0)
 end
end
expr %%lld $r
ret $r
