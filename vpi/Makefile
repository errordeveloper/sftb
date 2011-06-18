# iverilog-vpi hello.c
# iverilog -ohello.vvp hello.v
# vvp -M. -mhello hello.vvp

VERILOG_SIM	?= iverilog -g2005
VERILOG_VPI	?= iverilog-vpi

VERILOG_VPI_FLAGS = `pkg-config sndfile --libs --cflags`

PROJECT		 = sftb
PROJECT_SRC	?= $(PROJECT).c $(PROJECT).v
PROJECT_OUT	?= $(PROJECT).vpp $(PROJECT).vpi

simul: $(PROJECT_OUT)
	vvp -M. -m$(PROJECT) $(PROJECT).vpp

build: $(PROJECT_SRC)
	cd $(PROJECT_DIR)
	$(VERILOG_VPI) $(VERILOG_VPI_FLAGS) $(PROJECT).c
	$(VERILOG_SIM) -o$(PROJECT).vpp $(PROJECT).v
	rm -v -f $(PROJECT).o

$(PROJECT_OUT): build


clean:
	rm -v -f $(PROJECT_OUT)


%.new:
	mkdir `basename $@ .new`
	echo "include $(PROJECT_DIR)/Makefile" > `basename $@ .new`/Makefile
