#!/usr/bin/perl
use bignum;
unshift @INC,qq(./scripts);
require qq(io.pm);

sub readcodec{
	outl(0xbfe74018,($_[0]<<16)|0x80000000);
	while((inl(0xbfe74054)&1)==0){}
	return inl(0xbfe74018)&0xffff;
}

sub writecodec{
	outl(0xbfe74018,($_[0]<<16)|$_[1]);
	while((inl(0xbfe74054)&2)==0){}
}

sub test_codec
{
if(@ARGV==0)
{

for($i=0;$i<64;$i++)
{
	$val=readcodec($i);
	printf("%02x:%04x\n",$i,$val);
}

}
elsif(@ARGV==1)
{
	$a=hex($ARGV[0]);
	$val=readcodec $a; 
	printf("%02x:%04x\n",$a,$val);
}
elsif(@ARGV==2)
{
	$a=hex($ARGV[0]);
	$val=hex($ARGV[1]);
	writecodec($a,$val); 
}
}

sub play_music($$$)
{
	#reset codec
	outl(0xbfd01160,0x04000011); #orderaddr
	do_cmd("cacheflush 0x80000000 0x100000");
	do_cmd("cp0s;m8 16 2;mems;");
	do_cmd("put $_[0] 0xa8000000");
	#occ
	outl(0xbfe74004,$_[1]); #8bit
	writecodec(0x2c,44100); 
	writecodec(0x2,0x808); 
	writecodec(0x4,0x808); 
	writecodec(0x6,0x8); 
	writecodec(0xc,0x8); 
	writecodec(0x18,0x808); 
	writecodec(0x38,0x808); 
	writecodec(0x6a,0x241);

	outl(0xa4000000,0x04000001); #order
	outl(0xa4000004,0x08000000); #saddr
	outl(0xa4000008,0x0fe72420|(1<<31)|($_[2]<<30)|(2<<28)); #daddr
#	outl(0xa400000c,32); #length
	outl(0xa400000c,(-s $_[0])>>2); #length
	outl(0xa4000010,0); #step length
	outl(0xa4000014,1); #step times
	outl(0xa4000018,0x1001); #dmacmd
	outl(0xbfd01160,0x04000009); #orderaddr
}


sub rec_music()
{
	outl(0xbfe74058, 0x3);
	writecodec(0,0);  #extern enable vbr
	readcodec(0);
	writecodec(0x2a,1);  #extern enable vbr
	writecodec(0x1a,0);  #select record
	writecodec(0x2c,44100);  #pcm out rate
	writecodec(0x32,44100);  #pcm input rate
	writecodec(0x34,44100); #mic input rate
	outl(0xbfe74004,0x6363); #occ 8bit
	writecodec(0x2,0x808); #master vol
	writecodec(0x18,0x808); #pcm out vol
	outl(0xbfe74010,0x630000); #icc
	writecodec(0xe,0x35f); #mic vol
	writecodec(0x1c,0xf0f); #adc record gain
	writecodec(0x1e,0xf0f); #mic adc record gain
	writecodec(0x10,0x101); #line in volum
	outl(0xbfd01160,0x04000011); #orderaddr
	outl(0xbfd01160,0x04000012); #orderaddr

	my $rdesc=0xa4000000;

	outl($rdesc,($rdesc&0x1fffffff)|1); #order
	outl($rdesc+0x4,0x08000000); #saddr
	outl($rdesc+0x8,0x0fe74c4c|(1<<31)|(0<<30)|(1<<28)); #daddr
	outl($rdesc+0xc,8); #length
	outl($rdesc+0x10,0); #step length
	outl($rdesc+0x14,0x1000/8); #step times
	outl($rdesc+0x18,0x1); #dmacmd
	#outl(0xbfd01160,($rdesc&0x1fffffff)|0xa); #orderaddr
	#outl(0xbfd01160,0x04000009); #orderaddr
	outl(0xbfd01160,0x0400000a); #orderaddr
	sleep 1;
	do_cmd("d4 0xa8000000 10");
	sleep 1;
	do_cmd("m4 0xa8000000 1 2 3");
	sleep 1;
	do_cmd("d4 0xa8000000 10");
	my $desc=0xa4000100;

	outl($desc,($desc&0x1fffffff)|1); #order
	outl($desc+4,0x08000000); #saddr
	outl($desc+0x8,0x0fe72420|(1<<31)|(0<<30)|(2<<28)); #daddr
	outl($desc+0xc,0x1000); #length
	outl($desc+0x10,0); #step length
	outl($desc+0x14,1); #step times
	outl($desc+0x18,0x1001); #dmacmd
	outl(0xbfd01160,($desc&0x1fffffff)|0x9); #orderaddr
}


sub rec_and_play_music()
{
	outl(0xbfe74058, 0x3);
	writecodec(0,0);  #extern enable vbr
	readcodec(0);
	writecodec(0x2a,1);  #extern enable vbr
	writecodec(0x1a,0);  #select record
	writecodec(0x2c,44100);  #pcm out rate
	writecodec(0x32,44100);  #pcm input rate
	writecodec(0x34,44100); #mic input rate
	outl(0xbfe74004,0x6363); #occ 8bit
	writecodec(0x2,0x808); #master vol
	writecodec(0x18,0x808); #pcm out vol
	outl(0xbfe74010,0x630001); #icc
	writecodec(0xe,0x35f); #mic vol
	writecodec(0x1c,0xf0f); #adc record gain
	writecodec(0x1e,0xf0f); #mic adc record gain
	writecodec(0x10,0x101); #line in volum
	outl(0xbfd01160,0x04000011); #orderaddr
	outl(0xbfd01160,0x04000012); #orderaddr

	my $rdesc=0xa4000000;

	outl($rdesc,($rdesc&0x1fffffff)|1); #order
	outl($rdesc+0x4,0x08000000); #saddr
	outl($rdesc+0x8,0x0fe74c4c|(1<<31)|(0<<30)|(1<<28)); #daddr
	outl($rdesc+0xc,0x1000); #length
	outl($rdesc+0x10,0); #step length
	outl($rdesc+0x14,0x1); #step times
	outl($rdesc+0x18,0x1); #dmacmd
	#outl(0xbfd01160,($rdesc&0x1fffffff)|0xa); #orderaddr
	#outl(0xbfd01160,0x04000009); #orderaddr
	outl(0xbfd01160,0x0400000a); #orderaddr
	sleep 1;
	do_cmd("d4 0xa8000000 10");
	sleep 1;
	do_cmd("m4 0xa8000000 1 2 3");
	sleep 1;
	do_cmd("d4 0xa8000000 10");
	my $desc=0xa4000100;

	outl($desc,($desc&0x1fffffff)|1); #order
	outl($desc+4,0x08000000); #saddr
	outl($desc+0x8,0x0fe72420|(1<<31)|(0<<30)|(2<<28)); #daddr
	outl($desc+0xc,0x1000); #length
	outl($desc+0x10,0); #step length
	outl($desc+0x14,1); #step times
	outl($desc+0x18,0x1001); #dmacmd
	outl(0xbfd01160,($desc&0x1fffffff)|0x9); #orderaddr
}

sub rec_and_play_music_alc655()
{
	outl(0xbfe74000,1);
	sleep 1;
	outl(0xbfe74058, 0x3);
	writecodec(0,0);  #extern enable vbr
	readcodec(0);
	writecodec(0x2a,1);  #extern enable vbr
	writecodec(0x1a,0);  #select record
	writecodec(0x2c,44100);  #pcm out rate
	writecodec(0x32,44100);  #pcm input rate
	writecodec(0x34,44100); #mic input rate
	outl(0xbfe74004,0x6363); #occ 8bit

	printf "0x6a is 0x%x\n",readcodec(0x6a);
	writecodec(0x6a,readcodec(0x6a)|0x200); 
	printf "0x6a is 0x%x\n",readcodec(0x6a);
	writecodec(0x2,0x808); #master vol
	writecodec(0x18,0x808); #pcm out vol
	outl(0xbfe74010,0x610001); #icc
	writecodec(0xe,0x35f); #mic vol
	writecodec(0x1c,0xf0f); #adc record gain
	writecodec(0x1e,0xf0f); #mic adc record gain
	writecodec(0x10,0x101); #line in volum
	outl(0xbfd01160,0x04000011); #orderaddr
	outl(0xbfd01160,0x04000012); #orderaddr

	my $rdesc=0xa4000000;

	outl($rdesc,($rdesc&0x1fffffff)|1); #order
	outl($rdesc+0x4,0x08000000); #saddr
	outl($rdesc+0x8,0x0fe74c4c|(1<<31)|(0<<30)|(1<<28)); #daddr
	outl($rdesc+0xc,0x1000); #length
	outl($rdesc+0x10,0); #step length
	outl($rdesc+0x14,0x1); #step times
	outl($rdesc+0x18,0x1); #dmacmd
	#outl(0xbfd01160,($rdesc&0x1fffffff)|0xa); #orderaddr
	#outl(0xbfd01160,0x04000009); #orderaddr
	outl(0xbfd01160,0x0400000a); #orderaddr
	sleep 1;
	do_cmd("d4 0xa8000000 10");
	sleep 1;
	do_cmd("m4 0xa8000000 1 2 3");
	sleep 1;
	do_cmd("d4 0xa8000000 10");
	my $desc=0xa4000100;

	outl($desc,($desc&0x1fffffff)|1); #order
	outl($desc+4,0x08000000); #saddr
	outl($desc+0x8,0x0fe72420|(1<<31)|(0<<30)|(2<<28)); #daddr
	outl($desc+0xc,0x1000); #length
	outl($desc+0x10,0); #step length
	outl($desc+0x14,1); #step times
	outl($desc+0x18,0x1001); #dmacmd
	outl(0xbfd01160,($desc&0x1fffffff)|0x9); #orderaddr
}

sub convert8{
open F,q(/mnt/ramdisk1/root/1_8.wav);
open F1,q(>/tmp/1_8.wav);
while(!eof F)
{
	read F,$buf,1;
	@v=unpack("C*",$buf);
	print F1 pack("C*",$v[0]^0x80);
}
close F;
close F1;
}

sub convert16{
open F,q(/mnt/ramdisk1/root/1_16.wav);
open F1,q(>/tmp/1_16.wav);
while(!eof F)
{
	read F,$buf,2;
	@v=unpack("S*",$buf);
	print F1 pack("S*",$v[0]>>1);
}
close F;
close F1;
}


sub play_chain($$$)
{
	#reset codec
	outl(0xbfd01160,0x04000011); #orderaddr
	do_cmd("cacheflush 0x80000000 0x100000");
	do_cmd("'cp0s;m8 16 2;mems;'");
	do_cmd("put $_[0] 0xa8000000\n");
	#occ
	outl(0xbfe74004,$_[1]); #8bit
	writecodec(0x2c,44100); 
	writecodec(0x2,0x808); 
	writecodec(0x18,0x808); 

	my $desc=0xa4000000;

	outl($desc,$desc&0x1fffffff|1); #order
	outl($desc+4,0x08000000); #saddr
	outl($desc+8,0x0fe72420|(1<<31)|($_[2]<<30)|(2<<28)); #daddr
	outl($desc+0xc,0x100); #length
	outl($desc+0x10,0); #step length
	outl($desc+0x14,0); #step times
	outl($desc+0x18,0x1001); #dmacmd
	outl(0xbfd01160,$desc&0x1fffffff|9); #orderaddr
	<STDIN>;
	<STDIN>;

	my $pdesc=$desc;

	$desc=0xa4000100;

	outl($desc,$desc&0x1fffffff|1); #order
	outl($desc+4,0x08000000); #saddr
	outl($desc+8,0x0fe72420|(1<<31)|($_[2]<<30)|(2<<28)); #daddr
	outl($desc+0xc,(-s $_[0])>>2); #length
	outl($desc+0x10,0); #step length
	outl($desc+0x14,1); #step times
	outl($desc+0x18,0x1001); #dmacmd
	outl($pdesc,$desc&0x1fffffff|1); #order
}

#after play,write ac97 0x58
sub play_music_wreg($$$)
{
	#reset codec
	outl(0xbfd01160,0x04000011); #orderaddr
	do_cmd("cacheflush 0x80000000 0x100000");
	do_cmd("cp0s;m8 16 2;mems;");
	do_cmd("put $_[0] 0xa8000000\n");
	#occ
	outl(0xbfe74004,$_[1]); #8bit
	writecodec(0x2c,44100); 
	writecodec(0x2,0x808); 
	writecodec(0x18,0x808); 

	my $desc=0xa4000000;

	outl($desc,$desc&0x1fffffff|1); #order
	outl($desc+4,0x08000000); #saddr
	outl($desc+8,0x0fe72420|(1<<31)|($_[2]<<30)|(2<<28)); #daddr
	outl($desc+0xc,(-s $_[0])>>2); #length
	outl(0xbfe74004,$_[1]); #8bit
	outl($desc+0x10,0); #step length
	outl($desc+0x14,1); #step times
	outl($desc+0x18,0x1001); #dmacmd

	my $pdesc=$desc;

	$desc=0xa4000100;

	outl($desc,$desc&0x1fffffff|0); #order
	outl($desc+4,0x09000000); #saddr
	outl($desc+8,0x0fe74058|(1<<31)|(0<<30)|(2<<28)); #daddr
	outl($desc+0xc,1); #length
	outl($desc+0x10,0); #step length
	outl($desc+0x14,1); #step times
	outl($desc+0x18,0x1001); #dmacmd
	outl(0xa9000000,0x5a345a12);
	outl($pdesc,$desc&0x1fffffff|1); #order

	outl(0xbfd01160,$pdesc&0x1fffffff|9); #orderaddr
}

#play_music_wreg q(/srv/tftp/1.wav),0x6b6b,1;
#play_music q(/mnt/ramdisk1/root/1_8.wav),0x6363,0;
#play_music q(/mnt/ramdisk1/root/1_16.wav),0x6b6b,0;
#play_music q(/mnt/ramdisk1/root/1_16_44.wav),0x6b6b,1;
#play_music q(/mnt/ramdisk1/root/2_8.wav),0x6363,1;
#play_music q(/tmp/1_8.wav),0x6363,0;
#play_music q(/tmp/2.wav),0x6b6b,0;
#eval "@ARGV";
#rec_and_play_music
#rec_music
#test_codec
if(@ARGV)
{
eval "@ARGV";
}
else
{
#rec_and_play_music_alc655;
play_music q(/tftpboot/1_16_44.wav),0x6b6b,1;
}
