#!/usr/bin/perl

# ./getTree <num stream> <num_layer> <num dst> <bw> <seed>

use strict;

#`./getTree.pl 2 4 3 1 2`
#system "./getTree.pl 300 4 10 7 1";
#system "./getTree.pl 1 3 2 1 1";
#system "./getNodeCapacity 5 1";
system "./getNodeCap.pl 5 1";
#system "./getTree.pl 3 4 5 5 1";
system "./getTree.pl 300 4 10 5 1";
system "./algo 1";
system "./algo 2";
system "./algo 3";
