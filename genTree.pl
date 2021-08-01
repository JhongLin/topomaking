#!/usr/bin/perl
use strict;

sub dist{
        my $randNum;

        $randNum = int(rand(2));
        $randNum -= 1;

	if(($_[0] + $randNum) < 2){
		return 2;
	}

        return $_[0] + $randNum;
}

my @data;
my $line;
my @token;
my $max_node = -1;
my $i;
my $j;
my $num_args;
my %nodeMap;
my $num_dst;
my $node;
my $num_tree = 0;
my $ratio = 2;
my $total_dst = 0;
my $leaving_percent = 10;
my $is_leaving;

$num_args = $#ARGV + 1;
if ($num_args != 3) {
    print "\nUsage: genTree.pl num_of_trees node_cap num_of_dst\n";
    exit;

}

open(INPUT, "output.txt") || die "Cannot open output.txt\n";
@data = <INPUT>;
close INPUT;

open(OUTPUT, ">tree.txt");

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

if ($num_args == 3) {
	$num_tree = $ARGV[0];
}
else{
	$num_tree = int(($max_node + 1) / 2);
}

print "Num of tree = ".$num_tree."\n";

for($i = 0; $i < $num_tree; $i++){
	%nodeMap = "";
	$num_dst = int(rand($max_node / $ratio)) + 2;
	$node = int(rand($max_node + 1));

	if($num_dst > 150){
		$num_dst = 150;
	}

	$total_dst += $num_dst - 1;

	$nodeMap{$node} = 1;
	print OUTPUT "".$node;

	$num_dst = dist($ARGV[2]);
	#print "num dst = $num_dst\n";

	for($j = 0; $j < $num_dst; $j++){
		$is_leaving = rand(100);

		if($is_leaving < $leaving_percent && $j > 10){
			$node = int(rand($max_node + 1));
			while($nodeMap{$node} != 1){
				$node = int(rand($max_node + 1));
			}

			delete($nodeMap{$node});
			print OUTPUT "\t".(-1 * $node);
		}

		else{
			$node = int(rand($max_node + 1));
			while($nodeMap{$node} == 1){
				$node = int(rand($max_node + 1));
			}

			$nodeMap{$node} = 1;
			print OUTPUT "\t".$node;

		}

	}

	print OUTPUT "\n";	
}

system "./genNodeCapacity.pl ".$ARGV[1];#$total_dst";
#system "./genNodeCapacity.pl $total_dst";

close OUTPUT;
