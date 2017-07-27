#!/bin/bash -x
EJTAGEXE=${EJTAGEXE:=./ejtag_debug_pp} 
abisize=`${EJTAGEXE} setconfig core.abisize`

${EJTAGEXE} gdbserver 50010 1 </dev/null &

tmpfile=`mktemp /tmp/gdbXXX`
if [ "$abisize" = "00000040" ];then
echo  "set mips abi n64 " >> $tmpfile
else
echo  "set mips abi o32" >> $tmpfile
fi

echo >> $tmpfile <<AAA
define mysi
monitor si 0
c
end
AAA

echo  "target remote :50010" >> $tmpfile
mipsel-gdb -q -x $tmpfile $1
rm -f $tmpfile
