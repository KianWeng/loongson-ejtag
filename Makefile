OPTIONS_CC = -g -DHAVE_NVENV -DNVRAM_IN_FLASH
OPTIONS_LD = -g -ldl -lreadline 
export CC=gcc

all: ejtag_debug_usb ejtag_debug_pp

ejtag_debug_usb: begin.o main.o mips32.o usb_jtag.o usb_ejtag_command.o gdb.o get.o get64.o put.o put64.o disassemble.o example.o expr.o test.o end.o md5.o
	${CC} -g -lusb -o ejtag_debug_usb $^ -I . ${OPTIONS_LD}

ejtag_debug_pp: begin.o main.o mips32.o jtag.o parport.o gdb.o disassemble.o example.o end.o expr.o test.o md5.o
	${CC} -g  -o ejtag_debug_pp $^ -I . ${OPTIONS_LD}

begin.o main.o mips32.o jtag.o parport.o gdb.o disassemble.o get.o put.o expr.o test.o end.o md5.o:
	ar x ejtag.a $<

%.o:%.c
	./mycc -c -O2 -I. -I./lib -o $@ $< -I ./include -include common.h ${OPTIONS_CC}
clean:
	rm -f *.out *.o dev/*.o ejtag_debug_usb  ejtag_debug_pp kcore *.tmp.c lib/*.o lib/*.tmp.c lib/*.bin
