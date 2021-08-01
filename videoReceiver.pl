#!/usr/bin/perl

use IO::Socket::INET;
use strict;

sub getSec{
	my $timestamp = localtime(time);
	my @token = split(' ', $timestamp);
	my @time_token = split(':', @token[3]);
	
	return @time_token[0] * 3600 + @time_token[1] * 60 + @time_token[2];
}


my $socket;
my $recv_data = "";
my $total_byte = 0;
my $timestamp;
my $getFirst = 0;
my $first_pkt_time;
my $prev_sec = 0;
my $current_time;

$socket = new IO::Socket::INET (
LocalPort => '5408',
Proto => 'udp',
) or die "ERROR in Socket Creation : $!\n";

$timestamp = localtime(time);
print "time = ".getSec($timestamp)."\n";

while($recv_data ne "END"){
	$socket->recv($recv_data, 4096);

	$total_byte += length($recv_data);

	$current_time = getSec(localtime(time));

	if($getFirst == 0){
		$first_pkt_time = $current_time; 
		$getFirst = 1;
	}

	if($current_time != $prev_sec){
		print "".($prev_sec - $first_pkt_time)."\t".$total_byte."\n";
		$total_byte = 0;
		$prev_sec = $current_time;
	}
}

$socket->close();

print "total bytes = $total_byte\n";
