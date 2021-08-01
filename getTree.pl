#!/usr/bin/perl

# ./getTree <num stream> <num_layer> <num dst> <bw> <seed>

use strict;
my @data;
my @token;
my $line;
my $max_node = 0;
my $num_stream;
my $num_layer;
my $num_dst;
my $bw;
my $seed;
my $rand_num_dst;
my $rand_num;
my $i;
my $j;
my %used_nodes;
my $out;

my $num_args = $#ARGV + 1;
if ($num_args != 5) {
    print "\nUsage: ./getTree <num stream> <num_layer> <num dst> <bw> <seed>\n";
    exit;
}

$num_stream = $ARGV[0];
$num_layer = $ARGV[1];
$num_dst = $ARGV[2];
$bw = $ARGV[3];
$seed = $ARGV[4];


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

print "Max node = ".$max_node."\n";

open(OUTPUT, ">tree.txt");

srand($seed);
for($i = 0; $i < $num_stream; $i++){
	$out = "";
	%used_nodes =();

	$rand_num = int(rand($bw)) + 1;
	$out = "$i\t$rand_num";

	$rand_num = int(rand($max_node + 1));
	$used_nodes{$rand_num} = 1;

	$out = $out."\t$rand_num";

	$rand_num_dst = int(rand($num_dst)) + 1;
	for($j = 0; $j < $rand_num_dst; $j++){
		$rand_num = int(rand($max_node + 1));
		while(%used_nodes{$rand_num} != ""){
			$rand_num = int(rand($max_node + 1));
		}

		$used_nodes{$rand_num} = 1;
		$out = $out."\t$rand_num";
	}

	$rand_num_dst = int(rand($num_layer)) + 1;
	if($rand_num_dst < 2){
		$rand_num_dst = 2;
	}
	for($j = 0; $j < $rand_num_dst; $j++){
		print OUTPUT "$out\n";
	}
}	

close OUTPUT;
