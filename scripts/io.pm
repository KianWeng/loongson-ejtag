#!/usr/bin/perl
use bignum;
use Math::BigInt;
my $EXEFILE = defined($ENV{EJTAGEXE})?$ENV{EJTAGEXE}: q(ejtag_debug_usb);
sub do_cmd{
return do_cmds(@_);
}

sub do_cmd1{
my $cmd=shift @_;
my $newcmd =sprintf(qq($EXEFILE "$cmd"),@_);
my $ret=`$newcmd`;
exit -1 if $?;
return $ret;
}


sub do_cmd1v{
my $cmd=shift @_;
my $newcmd =sprintf(qq($EXEFILE "$cmd"),@_);
my $f;
my $ret;
open $f,qq($newcmd|);
while(<$f>)
{
print;
$ret .= $_;
}
exit -1 if $?;
close $f;
return $ret;
}

sub do_cmd2{
my $cmd=shift @_;
my $newcmd =sprintf(qq($EXEFILE "$cmd"),@_);
open $f,qq($newcmd|);
return $f;
}

sub do_cmds{
my $cmd=shift @_;
my @cmds =split /\n/,sprintf(qq($cmd),@_);

for $cmd (@cmds)
{
 if($cmd !~ /\S/) { next; }
 else
 {
 system(sprintf(qq($EXEFILE "$cmd")));
 }
}
exit -1 if $?;
}

sub ioall {
my $caller = ((caller(1))[3]);
my $cmd = $caller;
my %h=('b'=>'1','w'=>'2','l'=>'4','q'=>'8');
$cmd=~s/.*::out(l|w|b|q)/'m' . $h{$1}/e;
$cmd=~s/.*::in(l|w|b|q)/'d' . $h{$1} . 'q'/e;
	
if(substr($cmd,0,1) eq 'd')
{
my $buf=sprintf "$cmd %s 1\n",Math::BigInt->new($_[0])->as_hex();
my @v=split /\s/,`$EXEFILE $buf`;
exit -1 if $?;
return hex($v[0]);
}
else
{
my $buf=sprintf "$cmd %s %s\n",Math::BigInt->new($_[0])->as_hex(),Math::BigInt->new($_[1])->as_hex();
system qq($EXEFILE $buf);
exit -1 if $?;
return 0;
}
}

sub outq
{
return	ioall(@_);
}

sub inq
{
return ioall(@_);
}


sub outl
{
return	ioall(@_);
}

sub inl
{
return ioall(@_);
}


sub outw
{
return	ioall(@_);
}

sub inw
{
return ioall(@_);
}


sub outb
{
return	ioall(@_);
}

sub inb
{
return ioall(@_);
}

1;
