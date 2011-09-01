#!/bin/tcsh -f

if ($#argv != "0") then
  set tu = $argv[1]
else
  set tu = "sftb"
endif

setenv MODELSIM_PREFIX "/opt/Altera/11.0/modelsim_ase/"

set path = ( $MODELSIM_PREFIX/bin/ $path )

vlib work && vlog $tu.v && \
gcc -fPIC -m32 -c -g $tu.c \
  `pkg-config sndfile --libs --cflags` \
  -I$MODELSIM_PREFIX/include/ && \
  gcc -m32 -shared -o $tu.so $tu.o \
  `pkg-config sndfile --libs --cflags` && \
    vsim -c $tu -pli $tu.so # -lib work
