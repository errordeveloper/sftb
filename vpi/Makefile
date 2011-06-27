# iverilog-vpi hello.c
# iverilog -ohello.vvp hello.v
# vvp -M. -mhello hello.vvp

VERILOG_SIM	?= iverilog -g2005
VERILOG_VPI	?= iverilog-vpi
VERILOG_RUN	?= vvp


PROJECT		 = sftb
PROJECT_SRC	?= $(PROJECT).c $(PROJECT).v
PROJECT_OUT	?= $(PROJECT).vpp $(PROJECT).vpi $(PROJECT).cpp

## C-related macros will be appended to $(PROJECT).cpp,
## while the Verilog macros are going to the vpp file.

# C pre-processor macros:

ifndef DEBUG
DEBUG_MESSAGE = You can specify DEBUG flag to get verbose output.
else
PROJECT_DEF	 += -DDBG=$(DEBUG)
endif

# Testbench-specific macros:

ifdef OUTPUT
	VERILOG_DEF += -DOUTPUT=\"$(OUTPUT)\"
endif

ifdef PERIOD
	VERILOG_DEF += -DPERIOD=$(PERIOD)
endif

ifdef CYCLES
	VERILOG_DEF += -DCYCLES=$(CYCLES)
endif

# Example: make CYCLES=25 PERIOD=1

VERILOG_VPI_FLAGS = `pkg-config sndfile --libs --cflags` $(PROJECT_DEF)


.SUFFIXES: .c .v .vpp .vpi

.c.vpi:
ifdef DEBUG_MESSAGE
	${info $(DEBUG_MESSAGE)}
endif
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $<
	@echo "# `hostname` `date`" > $<pp
	@echo "# Compiled with:" >> $<pp
	@echo "# $(PROJECT_DEF)" >> $<pp

.v.vpp:
	$(VERILOG_SIM) $(VERILOG_DEF) -o$@ $<
	@echo "# `hostname` `date`" >> $@
	@echo "# Compiled with:" >> $@
	@echo "# $(VERILOG_DEF)" >> $@

%.rebuild_vpi: %.vpi %.c %.cpp
	@tail -n 1 $*.cpp | awk '{ \
		if ( $$0 != "# $(PROJECT_DEF)" ) { \
		system ("rm -f $<") \
		system ("make $<" ) } \
		else { \
		print ("No need to rebuild $<.") } }'

%.rebuild_vpp: %.vpp %.v
	@tail -n 1 $< | awk '{ \
		if ( $$0 != "# $(VERILOG_DEF)" ) { \
		system ("rm -f $<") \
		system ("make $<" ) } \
		else { \
		print ("No need to rebuild $<.") } }'

%.vcd: %.vpi %.vpp
	@make $(@:.vcd=.rebuild_vpp)
	$(VERILOG_RUN) -M. -m$(<:.vpi=) $*.vpp

build: $(PROJECT_OUT)

batch: $(PROJECT_SRC)
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $(PROJECT).c
	$(VERILOG_SIM) -o$(PROJECT).vpp $(PROJECT).v
	rm -f $(PROJECT).o
	$(VERILOG_RUN) -M. -m$(PROJECT) $(PROJECT).vpp

clean:
	rm -f $(PROJECT_OUT)
