# iverilog-vpi hello.c
# iverilog -ohello.vvp hello.v
# vvp -M. -mhello hello.vvp

VERILOG_SIM	?= iverilog -g2005
VERILOG_VPI	?= iverilog-vpi
VERILOG_RUN	?= vvp


PROJECT		 = sftb
PROJECT_SRC	+= $(PROJECT).c $(PROJECT).v
PROJECT_OUT	+= $(PROJECT).vpp $(PROJECT).vpi $(PROJECT)-debug.vpi $(PROJECT).vcd

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

# targets 'rebuild/file.vpi' and 'rebuild/file.vpp' are for internal use. 
# front-end target is 'file.vcd', if either PROJECT_DEF or VERILOG_DEF
# has changed, the vpi and vpp files will be rebuilt.

default:
	make $(PROJECT).vcd CYCLES=20 DEBUG=0

compile:
	make $(PROJECT).vpi

install: compile
ifndef INSTALL_LIB
	${error Need to define INSTALL_LIB path to install VPI module!}
else
	cp $(PROJECT).vpi $(INSTALL_LIB)
endif

#stamped:
#	make OUTPUT=sftb-`date +%d%m%y-%H%M`.vcd

#%.new:
#	cp $(PROJECT).v $(@:.new=.v)
#	ln $(PROJECT).c $(@:.new=.c)
#	vim $(@:.new=.v)

depends:
	@mkdir depends

.c.vpi:
ifdef DEBUG_MESSAGE
	${info $(DEBUG_MESSAGE)}
endif
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $<
	@rm -f $(<:.c=.o)
	@rm -f depends/$@
	@make depends/$@

.v.vpp:
	$(VERILOG_SIM) $(VERILOG_DEF) -o$@ $<
	@rm -f depends/$@
	@make depends/$@


depends/%.vpi: depends
	@echo "# `hostname` `date`" > $@
	@echo "# Compiled with:" >> $@
	@echo "# $(PROJECT_DEF)" >> $@

depends/%.vpp: depends
	@echo "# `hostname` `date`" > $@
	@echo "# Compiled with:" >> $@
	@echo "# $(VERILOG_DEF)" >> $@

rebuild/%.vpi: %.vpi %.c
	@tail -n 1 depends/$< | awk '{ \
		if ( $$0 != "# $(PROJECT_DEF)" ) { \
		system ("rm -f $<") \
		system ("make $<" ) } \
		else { \
		print ("No need to rebuild $<.") } }'

rebuild/%.vpp: %.vpp %.v
	@tail -n 1 depends/$< | awk '{ \
		if ( $$0 != "# $(VERILOG_DEF)" ) { \
		system ("rm -f $<") \
		system ("make $<" ) } \
		else { \
		print ("No need to rebuild $<.") } }'


%.vcd: %.vpi %.vpp
	@make rebuild/$*.vpi
	@make rebuild/$*.vpp OUTPUT=$(@)
	$(VERILOG_RUN) -M. -m$(<:.vpi=) $*.vpp

build: $(PROJECT_OUT)

batch: $(PROJECT_SRC)
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $(PROJECT).c
	$(VERILOG_SIM) -o$(PROJECT).vpp $(PROJECT).v
	rm -f $(PROJECT).o
	$(VERILOG_RUN) -M. -m$(PROJECT) $(PROJECT).vpp

clean:
	rm -f $(PROJECT_OUT)
	rm -rf depends
