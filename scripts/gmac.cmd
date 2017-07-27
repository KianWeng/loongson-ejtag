letl phy 16
letl base 0xffffffffbfe10000

function read_phy
@letl show $(@setconfig sys.showcmd)
@echo_off
letl reg ${1:0}
letl cnt ${2:1}
do while $cnt
letl data {($phy<<11)|($reg<<6)|1|0x10}
m4 {$base+0x10} $data
while {$(d4q {$base+0x10})&1};
expr_n %"0x%04x " $(d4q {$base+0x14} 1)
letl reg {$reg+1}
letl cnt {$cnt-1}
end
echo
setconfig sys.showcmd $show
@ret


function write_phy
@letl show $(@setconfig sys.showcmd)
@echo_off
letl reg ${1:0}
letl data ${2:0}
m4 {$base+0x14} $data
letl data {($phy<<11)|($reg<<6)|2|1|0x10}
m4 {$base+0x10} $data
while {$(d4q {$base+0x10})&1};
setconfig sys.showcmd $show
@ret



function print_speed
letl stat $1
letl lpa2 $(read_phy 0xa)
letl bmcr2 $(read_phy 0x9)
letl avert $(read_phy 0x4)
letl lpa $(read_phy 0x5)
letl nego {$lpa&$avert}
letl bmsr $(read_phy 1)

do if {$bmsr&4}
letl link "link up"
else
letl link "link down"
end


do if {$bmcr2&0x300&($lpa2>>2)}
letl speed "speed 1000M"
elsif {$nego&0x0180}
letl speed "speed 100M"
else 
letl speed "speed 10M"
end

do if {($lpa2&0x0800)|($nego&0x140)}
letl duplex "duplex full"
else
letl duplex "duplex half"
end

echo $link $speed $duplex
ret

function find_phy
letl phy0 -1
letl oldphy ${phy:0}
for letl i 0;{$i<32};letl i {$i+1}
let phy $i
letl v $(read_phy 2)
 do if {$v!=0&&$v!=0xffff} 
  echo find phy $phy
  print_speed
  letl phy0 $phy
 end
end
do if {$phy0!=-1}
let phy $phy0
echo phy is $phy
else
let phy $oldphy
echo can not find phy
end
ret

