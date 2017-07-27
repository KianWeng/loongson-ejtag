#!/bin/bash -x
EJTAGEXE=${EJTAGEXE:=./ejtag_debug_pp} 
abisize=`${EJTAGEXE} setconfig core.abisize`
if uname -a|grep -q mips;then
GDB=gdb
else
GDB=./mipsel-gdb
fi

${EJTAGEXE} gdbserver 50010 1 </dev/null &

tmpfile=`mktemp /tmp/gdbXXX`
if [ "$abisize" = "00000040" ];then
echo  "set mips abi n64 " >> $tmpfile
else
echo  "set mips abi o32" >> $tmpfile
fi
echo  "target remote :50010" >> $tmpfile
ddd --debugger ${GDB} -x $tmpfile $1
rm -f $tmpfile
