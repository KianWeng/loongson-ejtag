function gdb

letl m $(setconfig gdbserver.cpubitmap)
letl n $(setconfig core.cpuno)
while {((1<<$n)&$m)==0} letl n {$n+1}
setconfig core.cpuno $n

do if $(setconfig gdbserver.sim)
letl indebug 1
else
letl indebug {$(jtagregs d8q 10 1)&8}
end

letl ibc $(expr ($(d4q 0xffffffffff301000 1)>>24)&0xf)
letl dbc $(expr ($(d4q 0xffffffffff302000 1)>>24)&0xf)
if {!$indebug} cont

letl port $(gdbserver 0)
letl pid $(fork)
do if $pid
gdbserver $port 1
wait
else
> gdb.cmd echo_n
 do if $(expr $(setconfig core.abisize)==32)
>> gdb.cmd echo set architecture mips:isa32
>> gdb.cmd echo set mips abi o32
 else
>> gdb.cmd echo set architecture mips:isa64
>> gdb.cmd echo set mips abi n64
 end
>> gdb.cmd echo set remotetimeout 20
>> gdb.cmd echo set remote hardware-breakpoint-limit $ibc
>> gdb.cmd echo set remote hardware-watchpoint-limit $dbc
>> gdb.cmd echo define mysi
>> gdb.cmd echo monitor si 0
>> gdb.cmd echo c
>> gdb.cmd echo end
#>> gdb.cmd echo set debug remote -1
>> gdb.cmd echo target remote 127.0.0.1:$port
! ${GDB:./mipsel-gdb} -x gdb.cmd $@
exit 0
end
ret


function ddd
#setconfig gdbserver.sim 1
letl port $(gdbserver 0)
letl pid $(fork)
do if $pid
gdbserver $port 1
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
>> gdb.cmd echo set remote hardware-breakpoint-limit $ibc
>> gdb.cmd echo set remote hardware-watchpoint-limit $dbc
>> gdb.cmd echo define mysi
>> gdb.cmd echo monitor si 0
>> gdb.cmd echo c
>> gdb.cmd echo end
>> gdb.cmd echo target remote 127.0.0.1:$port
! ddd --debugger ./mipsel-gdb -x gdb.cmd $@
exit 0
end
ret

function fls
for letl num i "$1" 31;$(expr $i>=0);letl i $(expr $i-1)
 if $(expr $num&(1<<$i)) loop_break
end
echo $i
ret

function cache_config
letl prid config2 slines ssets sways $(cp0s 0 d4q 15 1) $(cp0s 2 d4q 16 1) 0 0 0

do if $(expr $prid==0x6305||$prid==0x00146308) 
letl slines ssets sways $(expr %"0x%x 0x%x 0x%x" 2<<(($config2>>4)&15) 64<<(($config2>>8)&15) 1+(($config2)&15)) 
letl cpucount $(setconfig core.cpucount)
letl ssize $(expr $ssets*$slines*$sways*$cpucount)
letl swaybit iwaybit dwaybit 0 0 0
setconfig scache.size $ssize
setconfig scache.ways $sways
setconfig scache.waybit $swaybit
end

letl config1 $(cp0s 1 d4q 16 1)
letl isets ilines iways $(expr  %"0x%x 0x%x 0x%x" 64<<(($config1>>22)&7) 2<<(($config1>>19)&7)  1+(($config1>>16)&7)) 
letl isize $(expr $isets*$ilines*$iways)
letl dsets dlines dways $(expr  %"0x%x 0x%x 0x%x" 64<<(($config1>>13)&7) 2<<(($config1>>10)&7) 1+(($config1>>7)&7)) 
letl dsize $(expr $dsets*$dlines*$dways)

if $(expr $prid!=0x6305) letl dwaybit $(fls {$dsets*$dlines})

setconfig icache.size $isize
setconfig icache.ways $iways
setconfig icache.waybit $iwaybit
setconfig dcache.size $dsize
setconfig dcache.ways $dways
setconfig dcache.waybit $dwaybit

ret


function cache_init
setconfig core.nocache 0
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
ret

function eclipse
#setconfig gdbserver.sim 1

letl m $(setconfig gdbserver.cpubitmap)
letl n $(setconfig core.cpuno)
while {((1<<$n)&$m)==0} letl n {$n+1}
setconfig core.cpuno $n

do if $(setconfig gdbserver.sim)
letl indebug 1
else
letl indebug {$(jtagregs d8q 10 1)&8}
end

letl ibc $(expr ($(d4q 0xffffffffff301000 1)>>24)&0xf)
letl dbc $(expr ($(d4q 0xffffffffff302000 1)>>24)&0xf)
if {!$indebug} cont

letl port $(gdbserver 0)
letl pid $(fork)
do if $pid
gdbserver $port 1
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
>> gdb.cmd echo set remote hardware-breakpoint-limit $ibc
>> gdb.cmd echo set remote hardware-watchpoint-limit $dbc
>> gdb.cmd echo define mysi
>> gdb.cmd echo monitor si 0
>> gdb.cmd echo c
>> gdb.cmd echo end
>> gdb.cmd echo define myedit
>> gdb.cmd echo set listsize 1
>> gdb.cmd echo edit
>> gdb.cmd echo set listsize 10
>> gdb.cmd echo end
>> gdb.cmd echo target remote 127.0.0.1:$port
! eclipse -data workspace/
exit 0
end
ret

function eclipse_tmp
> gdb.cmd echo_n
 do if $(expr $(setconfig core.abisize)==32)
>> gdb.cmd echo set mips abi o32
 else
>> gdb.cmd echo set mips abi n64
 end
do if $(expr $(setconfig core.cpuwidth)==32)
>> gdb.cmd echo set architecture mips:isa32r2
else
>> gdb.cmd echo set architecture mips:isa64r2
end

! eclipse -data workspace/
ret


function gdbmod0

do if $(setconfig gdbserver.sim)
letl indebug 1
else
letl indebug {$(jtagregs d8q 10 1)&8}
end
letl ibc $(expr ($(d4q 0xffffffffff301000 1)>>24)&0xf)
letl dbc $(expr ($(d4q 0xffffffffff302000 1)>>24)&0xf)
if {!$indebug} cont


letl port $(gdbserver 0)
letl pid $(fork)
do if $pid
gdbserver $port 1
wait
else

> gdb.cmd echo_n
 do if $(expr $(setconfig core.abisize)==32)
>> gdb.cmd echo set architecture mips:isa32
>> gdb.cmd echo set mips abi o32
 else
>> gdb.cmd echo set architecture mips:isa64
>> gdb.cmd echo set mips abi n64
 end
>> gdb.cmd echo set remotetimeout 20
>> gdb.cmd echo set remote hardware-breakpoint-limit $ibc
>> gdb.cmd echo set remote hardware-watchpoint-limit $dbc
>> gdb.cmd echo define mysi
>> gdb.cmd echo monitor si 0
>> gdb.cmd echo c
>> gdb.cmd echo end
>> gdb.cmd echo target remote 127.0.0.1:9000
>> gdb.cmd echo source scripts/gdb.func

 do if $(expr $(setconfig core.abisize)==32)
! perl scripts/gdb.proxy.32 $@
 else
! perl scripts/gdb.proxy.64 $@
 end
! ${GDB:./mipsel-gdb} -x gdb.cmd $@
exit 0
end
ret

function gdbmod
#setconfig gdbserver.sim 1

do if $(setconfig gdbserver.sim)
letl indebug 1
else
letl indebug {$(jtagregs d8q 10 1)&8}
end
letl ibc $(expr ($(d4q 0xffffffffff301000 1)>>24)&0xf)
letl dbc $(expr ($(d4q 0xffffffffff302000 1)>>24)&0xf)
if {!$indebug} cont

letl port $(gdbserver 0)
letl pid $(fork)
do if $pid
gdbserver $port 1
wait
else

> gdb.cmd echo_n
 do if $(expr $(setconfig core.abisize)==32)
>> gdb.cmd echo set architecture mips:isa32
>> gdb.cmd echo set mips abi o32
 else
>> gdb.cmd echo set architecture mips:isa64
>> gdb.cmd echo set mips abi n64
 end
>> gdb.cmd echo set remotetimeout 20
>> gdb.cmd echo set remote hardware-breakpoint-limit $ibc
>> gdb.cmd echo set remote hardware-watchpoint-limit $dbc
>> gdb.cmd echo define mysi
>> gdb.cmd echo monitor si 0
>> gdb.cmd echo c
>> gdb.cmd echo end
>> gdb.cmd echo target remote 127.0.0.1:9000
>> gdb.cmd echo source scripts/gdb1.py

 do if $(expr $(setconfig core.abisize)==32)
! python scripts/gdb.proxy.32.py $@
 else
! python scripts/gdb.proxy.64.py $@
 end
! ${GDB:./mipsel-gdb} -x gdb.cmd $@
exit 0
end
ret

function check_ejtag
let l $(jtagregs d4q 1 1)

do if {$l!=0x5a5a5a5a&&$l!=0x20010819}
echo ejtag link failed
;ret
end

for letl i 0;{$i<10000}; letl i {$i+1};
let d $(jtagregs d4q 1 1)
if {$d!=$l} loop_break
end

do if {$d==$l}
echo check ok
else
echo check failed, read $l as $d on $i 
end

ret

function devmem
newfunc f_d "echo_2 devmem \{$$2&0x1fffffff} \{%%d $$1*8} $$3 $$4"
newfunc f_dq "echo_2 devmem \{$$2&0x1fffffff} \{%%d $$1*8}; echo $$RANDOM;"
newcmd d1 'f_d 1'
newcmd d2 'f_d 2'
newcmd d4 'f_d 4'
newcmd d8 'f_d 8'
newcmd d1q 'f_dq 1'
newcmd d2q 'f_dq 2'
newcmd d4q 'f_dq 4'
newcmd d8q 'f_dq 8'
newcmd m1 'f_d 1'
newcmd m2 'f_d 2'
newcmd m4 'f_d 4'
newcmd m8 'f_d 8'
ret

function dummy
newfunc f_dq "echo_2 $$1 $$2; echo $$RANDOM;"
newcmd d1 'echo_2 d1'
newcmd d2 'echo_2 d2'
newcmd d4 'echo_2 d4'
newcmd d8 'echo_2 d8'
newcmd d1q 'f_dq d1q'
newcmd d2q 'f_dq d2q'
newcmd d4q 'f_dq d4q'
newcmd d8q 'f_dq d8q'
newcmd m1 'echo_2 m1'
newcmd m2 'echo_2 m2'
newcmd m4 'echo_2 m4'
newcmd m8 'echo_2 m8'
ret

function myput64
letl f ${1:/srv/tftp/vmlinux}
letl l $(test -s $f)
letl b 0x80000
for letl o 0;{$o<$l};letl o {$o+$b}
stop
put64 $f {0xffffffff88000000+$o} $b $o
go
msleep 2000
end
ret

function jtag_clk
letl clk ${1:1}
usblooptest 0x81000070  {0x10000|$clk}
ret
