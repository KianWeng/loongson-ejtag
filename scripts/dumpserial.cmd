function dumpserial
mems
watch ${1:0xffffffffbfe40000}
cont

do while 1;
while {($(jtagregs d8q 10 1)&8)==0};

letl c $(set {($(d4q $(set pc) 1)>>16)&0x1f});
if {$c!=0x1b} expr_n  %%c $c
m4 0xFF302000 0
cont
end
ret

function dumpserial.hb
letl saddr ${1:0xffffffffbfe001e0}
mems
watch $saddr
cont

do while 1;
while {($(jtagregs d8q 10 1)&8)==0};

letl c $(set {($(d4q $(set pc) 1)>>16)&0x1f});
if {$c!=0x1b} expr_n  %%c $c
#m4 0xffffffffFf302000 0
unwatch $saddr
letl npc $(nextpc)
hb $npc
cont
while {($(jtagregs d8q 10 1)&8)==0};
unhb $npc
watch $saddr
cont
end
ret

function watchprint
echo_off
mems
watch ${1:0xffffffffbfe40000} ${2:0}
cont

do while 1;
while {($(jtagregs d8q 10 1)&8)==0};

letl ins $(d4q $(set pc) 1)
letl rt {($ins>>16)&0x1f}
letl base {($ins>>21)&0x1f}
letl offs {$ins&0xffff}
letl offs {($offs&0x8000)?((~0xffff)|$offs):$offs}
letl a {$(set $base)+$offs}
letl v $(set $rt)
expr %"m4 0x%x 0x%x" $a $v
m4 0xFF302000 0
cont
end
ret

function awatchprint
echo_off
mems
awatch ${1:0xffffffffbfe40000} ${2:0}
cont

do while 1;
while {($(jtagregs d8q 10 1)&8)==0};

letl ins $(d4q $(set pc) 1)
letl rt {($ins>>16)&0x1f}
letl base {($ins>>21)&0x1f}
letl offs {$ins&0xffff}
letl offs {($offs&0x8000)?((~0xffff)|$offs):$offs}
letl a {$(set $base)+$offs}
do if {$ins&0x20000000}
letl v $(set $rt)
expr %"m4 0x%x 0x%x" $a $v
else
letl v $(d4q $a 1)
expr %"d4q 0x%x 1 ;# 0x%x" $a $v
end
m4 0xFF302000 0
cont
end
ret


function mywatch
echo_off
letl saddr ${1:0x900000000ff00068}
letl val ${2:0}
letl mypc ${3:1}
mems
watch $saddr
cont

do while 1;
while {($(jtagregs d8q 10 1)&8)==0};

letl pc $(set pc)
letl ins $(d4q $pc 1)
letl rt {($ins>>16)&0x1f}
letl base {($ins>>21)&0x1f}
letl offs {$ins&0xffff}
letl offs {($offs&0x8000)?((~0xffff)|$offs):$offs}
letl a {$(set $base)+$offs}
letl v $(set $rt)
do if {$#==3||$pc==$mypc}
set $rt $val
expr %"%s:m8 0x%llx 0x%016llx;#0x%016llx" '$(symbol show $pc)' $a $v $val
else
expr %"%s:m8 0x%llx 0x%016llx" '$(symbol show $pc)' $a $v
end
unwatch $saddr
letl npc $(nextpc)
hb $npc
cont
while {($(jtagregs d8q 10 1)&8)==0};
unhb $npc
watch $saddr
cont
end
ret

