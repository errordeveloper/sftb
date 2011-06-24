# iverilog-vpi hello.c
# iverilog -ohello.vvp hello.v
# vvp -M. -mhello hello.vvp

VERILOG_SIM	?= iverilog -g2005
VERILOG_VPI	?= iverilog-vpi
VERILOG_RUN	?= vvp


PROJECT		 = sftb
PROJECT_SRC	?= $(PROJECT).c $(PROJECT).v
PROJECT_OUT	?= $(PROJECT).vpp $(PROJECT).vpi

ifndef DEBUG
${info You can specify DEBUG flag to get verbose output.}
else
PROJECT_DBG	 = -DDBG=$(DEBUG)
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

VERILOG_VPI_FLAGS = `pkg-config sndfile --libs --cflags` $(PROJECT_DBG)


.SUFFIXES: .c .v .vpp .vpi

.c.vpi:
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $<

TAIL:= `tail -1 sftb.vpp | sed -e "s/\# //"`

.v.vpp:
	$(VERILOG_SIM) $(VERILOG_DEF) -o$@ $<
	@echo "# Compiled with:" >> $@
	@echo "# $(VERILOG_DEF)" >> $@


%.new: %.vpp %.v
	@tail -n 1 $< | awk '{ \
		if ( $$0 != "# $(VERILOG_DEF)" ) { \
		print("The constants differ.\n") \
		system ("rm -v -f $<") \
		system ("$(VERILOG_SIM) -o$< $(VERILOG_DEF) $*.v") \
		system ("echo \"# Compiled with\" >> $<" ) \
		system ("echo \"# $(VERILOG_DEF)\" >> $<" ) } \
		else { \
		print("No need rebuild $<.") } }'


%.vcd: %.vpi %.vpp
	@awk 'BEGIN { x = 0 } { \
	       	if ( $$1 == "%vpi_call" \
		  && $$4 == "\"$$dumpfile\"," \
		  && $$5 != "\"$@\"" ) \
		{ x++ } } END { \
	       	if ( x > 0 ) { \
		  system ("rm -v -f $*.vpp") \
		  system ("make $*.vpp OUTPUT=$@") \
		} }' $*.vpp
	$(VERILOG_RUN) -M. -m$(<:.vpi=) $*.vpp

build: $(PROJECT_OUT)

batch: $(PROJECT_SRC)
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $(PROJECT).c
	$(VERILOG_SIM) -o$(PROJECT).vpp $(PROJECT).v
	rm -v -f $(PROJECT).o
	$(VERILOG_RUN) -M. -m$(PROJECT) $(PROJECT).vpp

clean:
	rm -v -f $(PROJECT_OUT)
