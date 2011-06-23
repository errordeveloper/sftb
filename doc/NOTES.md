Simulation time for 1000000 cycles.
===================================

	% time make DEBUG=1
	8.025u 5.643s 0:19.55 69.8%	0+0k 3328+110568io 0pf+0w
	
	% time make DEBUG=0
	$sftb_open_input_file: opened '/tmp/sunday.aiff'.
	$sftb_open_input_file: this file has 7779828 frames.
	VCD info: dumpfile dump.vcd opened for output.
	$sftb_close_input_file: Closing input file '0'.
	3.766u 0.177s 0:03.99 98.4%	0+0k 0+110560io 0pf+0w
	
	% du -hs dump.vcd 
	54M	dump.vcd

