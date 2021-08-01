#!/usr/bin/perl
use strict;
my @data;
my $line;
my @token;
my $line_count = 0;
my $num_node;
my $count = 0;
my $cost;

open(INPUT, "./inet-3.0/out.txt") || die "Cannot open the file";
@data = <INPUT>;
close INPUT;

foreach $line(@data){
	chomp $line;


	@token = split(' ', $line);

	if($line_count == 0){
		$num_node = @token[0];
	}

	if($line_count > $num_node){
		$cost = int(@token[2] / 1000);
		if($cost == 0){
			$cost = 1;
		}

		$cost = 1;

		print "$count\t".@token[0]."\t".@token[1]."\t$cost\n";
		$count++;
	}

	$line_count++;
}
