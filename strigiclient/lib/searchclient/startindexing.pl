#! /usr/bin/perl
use strict;
use Socket;

my $query = join(" ", @ARGV);

# define the socket location
my $rendezvous = "$ENV{HOME}/.strigi/socket";
# 
socket(SOCK, PF_UNIX, SOCK_STREAM, 0)       || die "socket: $!";
select(SOCK);
$|=1;
select(STDOUT);
connect(SOCK, sockaddr_un($rendezvous))     || die "connect: $!";

my $offset = 0;
my $maxresults = 10;

# send the request
print SOCK "startIndexing\n\n";

# read the result
my $line;
while (defined($line = <SOCK>)) {
    print $line;
}
close(SOCK) || die "close: $!";
