SoundFile TestBench
===================

##About:

I had been trying to find a way to supply audio test data
to a Verilog HDL simulator and could not find anything on
the net. One could use file IO system task in Verilog, but
it is cumbersome and it would be hard to provide support
for various audio file formats out there. There many audio
file libraries out there, though `libsndfile` appears to
be the most robust options :)

##Usage:

###To install:

You will need _icarus_ Verilog simulator and _libsndfile_
to use this tesbench. Other Verilog simulator to be added.

`git clone git://github.com/errordeveloper/sftb sftb.git`

`cd ./sftb.git/vpi/; make defualt`

`gtkwave sftb.vcd`

You will probably need to edit `sftb.v` to provide the
filename or place a file `/tmp/sunday.aiff` and it will
run fine. There quite a few things still to fix, I hope
to get it brushed-up very soon!

![_Screenshow of the results_][1]

[1]: http://i.stack.imgur.com/ICSz2.png "Sunday Morning"


##To-do:
	* Test with Verilator
	* Implement DPI version
	* Proper documentation
	* Filer designs
