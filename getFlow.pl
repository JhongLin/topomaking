#!/usr/bin/perl

# ./getTree <num stream> <num_layer> <num dst> <bw> <seed>

use strict;
my @data;
my @token;
my $line;
my $max_node = 0;
my $num_flow;
my $seed;
my $rand_num;
my $i;
my $out;

my $num_args = $#ARGV + 1;
if ($num_args != 2) {
    print "\nUsage: ./getTree <num stream> <num_layer> <num dst> <bw> <seed>\n";
    exit;
}

$num_flow = $ARGV[0];
$seed = $ARGV[1];

open(INPUT, "output.txt") || die "Cannot open the file\n";
@data = <INPUT>;
close INPUT;

foreach $line(@data){
	chomp $line;

	@token = split(' ', $line);
	if(@token[1] > $max_node){
		$max_node = @token[1];
	}

	if(@token[2] > $max_node){
		$max_node = @token[2];
	}
}


open(OUTPUT, ">flow.txt");

srand($seed);
for($i = 0; $i < $num_flow; $i++){
	$out = "";
	
	$rand_num = int(rand($max_node + 1));


	$out .= $rand_num;

	$rand_num = int(rand($max_node + 1));


	$out .= "\t".$rand_num;

	print OUTPUT "$out\n";
}	

close OUTPUT;
