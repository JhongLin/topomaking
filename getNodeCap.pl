#!/usr/bin/perl

# ./getTree <num stream> <num_layer> <num dst> <bw> <seed>

use strict;
my @data;
my @token;
my $line;
my $max_node = 0;
my $cap;
my $seed;
my $i;
my $rand_num;

my $num_args = $#ARGV + 1;
if ($num_args != 2) {
    print "\nUsage: ./getTree <node capacity> <seed>\n";
    exit;
}

$cap = $ARGV[0];
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

srand($seed);


open(OUTPUT, ">nodeCap.txt");

for($i = 0; $i <= $max_node; $i++){
	$rand_num = int(rand($cap)) + 1;
	print OUTPUT "$i\t$rand_num\n";
}

close OUTPUT;
