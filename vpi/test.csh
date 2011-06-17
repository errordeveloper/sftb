#!/bin/tcsh -f
iverilog-vpi $argv[1].c -lsndfile -I/opt/local/include/
iverilog -o${argv[1]}.vvp $argv[1].v
rm -f $argv[1].o
vvp -M. -m${argv[1]} $argv[1].vvp

printf "Clean-up the output files? (y/n):"
switch($<)
	case y:
	rm -f ${argv[1]}.v??
	breaksw
	case n:
	default
	breaksw
endsw
