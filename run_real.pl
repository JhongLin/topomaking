#!/usr/bin/perl

# ./getTiree <num stream> <num_layer> <num dst> <bw> <seed>

use strict;

sub numOfNodes{
	my @data;
	my $line;
	my @token;
	my $max = 0;
	my $cost;

	open(INPUT, @_[0]) || die "Cannot open the file";
	@data = <INPUT>;
	close INPUT;

	open(OUTPUT, ">output.txt");

	foreach $line(@data){
		chomp $line;
		@token = split(' ', $line);

		if(@token[1] > $max){
			$max = @token[1];
		}
		if(@token[2] > $max){
			$max = @token[2];
		}

		$cost = int(rand(10)) + 1;

		print OUTPUT "".@token[0]."\t".@token[1]."\t".@token[2]."\t".$cost."\n";

	}

	close OUTPUT;

	return ($max + 1);
}

my $dirname;
my $filename;
my $count = 0;
my $numNode;

$dirname = "/home/kent/multicast_svc/topo_ok";

opendir ( DIR, $dirname ) || die "Error in opening dir $dirname\n";
while( ($filename = readdir(DIR))){
	next if($filename eq "." || $filename eq "..");

	$numNode = numOfNodes("$dirname/$filename");

	if($numNode >= 100){
		print("-----------------  $filename  ------------------\n");
		print "num of nodes = ".$numNode."\n";		


		system("./getNodeCap.pl 2 1");
		system "./getTree.pl 2 3 5 3 1";
		system "./algo 1";
		system "./algo 2";
		system "./algo 3";
	}
	
	#last if($count >= 0);
	#$count++;
}
closedir(DIR);

#system "./getTree.pl 300 4 10 7 1";
#system "./algo 1";
#system "./algo 2";
#system "./algo 3";
