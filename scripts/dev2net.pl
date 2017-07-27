#!/usr/bin/perl
use warnings;
use Socket;
use Fcntl;
use IO::Select;
use IO::Socket;
use Math::BigInt;
use bignum;
#use bignum qw/hex oct/;
use Socket qw(IPPROTO_TCP TCP_NODELAY);

sub set_unblock
{
my $fd = $_[0];
		if ($^O eq 'MSWin32')
		{
			my $nonblocking = pack('L', 1);
			ioctl($fd, 0x8004667e, unpack('I', pack('P', $nonblocking)));
		}
		else
		{
			fcntl($fd, F_SETFL, fcntl($socket, F_GETFL, 0) | O_NONBLOCK);
		}
}




if(@ARGV > 0)
{
	if( -e $ARGV[0])
	{
		sysopen $socket,$ARGV[0], O_NONBLOCK|O_RDWR;
		system qq( stty speed 115200  < $ARGV[0]);
		#set_unblock($socket);
	}
	else
	{
		$socket = IO::Socket::INET->new(PeerAddr => $ARGV[0]);
		setsockopt($socket, IPPROTO_TCP, TCP_NODELAY, 1);
	}
}
else
{
	die "$0 {file|port}";
}

$server = IO::Socket::INET->new(Listen    => 5, LocalAddr => 'localhost', LocalPort => 50010, Proto     => 'tcp',ReuseAddr=>1);
$client = $server->accept();
setsockopt($client, IPPROTO_TCP, TCP_NODELAY, 1);

$|=1;


$sel = IO::Select->new( ($socket,$client) );



TOP: while(@ready = $sel->can_read) {
	foreach $fh (@ready) {
		if($fh == $socket) {
			$ret = sysread($socket, $buf, 4096);

			if(defined($ret))
			{
				last TOP if !length($buf);
				print $client "$buf";
				print qq(q: $buf\n);
			}
		}
		else {
			$ret = recv($client, $buf, 4096, MSG_DONTWAIT);
			if(defined($ret))
			{

				last TOP if !length($buf);

				syswrite $socket, $buf;
				print qq(g: $buf\n);
			}
		}
	}
}



close($socket);
close($client);
close($server);

