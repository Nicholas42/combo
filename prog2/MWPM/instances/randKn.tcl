#!/usr/bin/tclsh

set maxweight 1000
if {$argc <= 0} {
    puts "Usage: ./randKn.tcl <num vertices (even!)> <maximum weight (int) default = $maxweight>"
    return 1;
}
if {$argc == 2} {
    set maxweight [lindex $argv 1]
}

set nnodes [lindex $argv 0]
set parity [expr {$nnodes % 2}]
if {$parity ==1} {
    puts "odd number of vertices cannot have a perfect matching."
    return 1;
}

set nedges [ expr {$nnodes * ($nnodes-1) / 2} ]

puts "p edge $nnodes $nedges"
set v 1
while {$v <= $nnodes} {
    set w [expr {$v+1}]
    while {$w <= $nnodes} {
	set weight  [expr {$maxweight/2 - int(rand() * $maxweight)}]
	puts "e $v $w $weight"
	incr w
    }
    incr v

}
