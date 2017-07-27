#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);
my $gmac_base=0xffffffffbfe10000;
my $phy = 0;

sub read_phy_reg
{
my $phy1 = @_>1?$_[1]:$phy;
my $reg = $_[0];
my $data;
 $data = ($phy1<<11)|($reg<<6)|1|0xc;
 outl($gmac_base + 0x10,$data);
 while(inl($gmac_base + 0x10) &1){};
 $data = inl($gmac_base + 0x14) & 0xffff;
 return $data;
}

sub write_phy_reg
{
my $reg = $_[0];
my $data = $_[1];

 outl($gmac_base + 0x14,$data);

 $data = ($phy<<11)|($reg<<6)| 2 | 1 | 0xc;
 outl($gmac_base + 0x10,$data);
 while(inl($gmac_base + 0x10) &1){};
}

sub read_all
{
for($i=0;$i<32;$i++)
{
print qq(\n) if(($i&0xf) == 0);
printf("%04x ",read_phy_reg($i));
}
print qq(\n);
my $stat = read_phy_reg(17);
print qq(Link up\n) if($stat & 0x200);

my %speed = ( 0=>"10M",1=>"100M",2=>"1000M",3=>"unknow speed" );
my $speed = $speed{$stat>>14};
my $duplex = (($stat>>13)&1)?"duplex full":"duplex half";
my $link = (($stat>>10)&1)?"link up":"no link";
my $autoneg = (($stat>>11)&1)?"resolved":"unresolved";
my $crossover = (($stat>>6)&1)?"corssover":"";

print qq($speed $duplex $link $autoneg $crossover\n);



}

sub MDC_MDIO_CTRL0_REG          {31;}
sub MDC_MDIO_CTRL1_REG          {21;}
sub MDC_MDIO_ADDRESS_REG        {23;}
sub MDC_MDIO_DATA_WRITE_REG     {24;}
sub MDC_MDIO_DATA_READ_REG      {25;}

sub MDC_MDIO_ADDR_OP           {0x000E;}
sub MDC_MDIO_READ_OP    {0x0001;}
sub MDC_MDIO_WRITE_OP   {0x0003;}

sub smi_write
{
write_phy_reg(MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);
write_phy_reg(MDC_MDIO_ADDRESS_REG, $_[0]);
write_phy_reg(MDC_MDIO_DATA_WRITE_REG, $_[1]);
write_phy_reg(MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);
}

sub smi_read
{
write_phy_reg(MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);
write_phy_reg(MDC_MDIO_ADDRESS_REG, $_[0]);
write_phy_reg(MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);
return read_phy_reg(MDC_MDIO_DATA_READ_REG);
}

sub RTL8370_REG_CHIP_DEBUG0    {0x1303;}
sub RTL8370_REG_CHIP_DEBUG1    {0x1304;}
sub RTL8370_REG_DIGITIAL_INTERFACE_SELECT    {0x1305;}
sub RTL8370_SELECT_RGMII_0_MASK { 0x7; }
sub RTL8370_SELECT_RGMII_1_OFFSET { 0x4; }

sub rtl8370_setAsicRegBits
{
my ($reg,$bits,$value)=@_;
my $v = smi_read($reg);

   for($shift = 0; !($bits & (1<<$shift));$shift++){};
     smi_write( $reg,($v & ~$bits) | (($value<<$shift)&$bits));

}

sub rtl8370_setAsicPortExtMode
{
my $id = $_[0];
my $mode = $_[1];
 smi_write(RTL8370_REG_CHIP_DEBUG0,0x0367);
 smi_write(RTL8370_REG_CHIP_DEBUG1,0x7777);
 rtl8370_setAsicRegBits(RTL8371_REG_DIGITIAL_INTERFACE_SELECT, RTL8371_SELECT_RGMII_0_MASK<<($id*RTL8370_SELECT_RGMII_1_OFFSET), $mode);
}

sub macForceLinkExt01_set
{
 rtl8370_setAsicPortExtMode(0,1);
 rtl8370_setAsicPortExtMode(1,1);
 smi_write(0x1310, 0x1076);
 smi_write(0x1311, 0x1076);
}

sub enable_gmac1
{
smi_write() 
}

sub find_phy
{
 my $v;
 my $i;
for($i=0;$i<32;$i++)
{
 $v = read_phy_reg(2,$i);
 if($v!=0 && $v!=0xffff)
 {
 printf("find phy id is %d\n", $i);
 $phy = $i;
 last;
 }
}

 if($i==32)
 {
 printf("can not find phy\n");
 $phy = 1;
 }
}


sub getasicreg
{
my $addr=$_[0];
my $data;
printf("add is 0x%x\n", $addr);
write_phy_reg(31,0xe);
write_phy_reg(23,$addr);
write_phy_reg(21,1);
$data = read_phy_reg(25);
printf("%x\n", $data);
return $data;
}


#write_phy_reg(0,0);
if (@ARGV==0)
{
print qq(gmac1 detect phy\n);
$gmac_base=0xffffffffbfe10000;
$phy=0;
find_phy;
read_all;
print qq(gmac2 detect phy\n);
$gmac_base=0xffffffffbfe20000;
$phy=0;
find_phy;
read_all;
}
else
{
find_phy;
	eval "@ARGV";
}

