TOPDIR=$(shell pwd)
BUILDDIR=$(TOPDIR)
obj=$(BUILDDIR)
src=$(TOPDIR)

export TOPDIR BUILDDIR obj src


srcs := $(shell echo *.c|sed 's/\stest[^.]*\.c/ /g' ) test.c
ifneq ($(LOADADDR), 0)
bins := $(patsubst %.c,%.bin64$(affix),$(srcs))
else
bins := $(patsubst %.c,%.bin$(affix),$(srcs)) $(patsubst %.c,%.bin64$(affix),$(srcs)) $(shell echo *.S|sed 's/bin.lds.S//;s/bin64.lds.S//;s/start.S//;s/start64.S//;'|sed 's/[.]S/.BIN64/g')
endif

all: clean $(bins)
ifeq ($(LOADADDR), 0)
	make LOADADDR=0x980000001fc00000 clean all
	make LOADADDR=0x8000f000 clean  guess.bin-0x8000f000  guess1.bin-0x8000f000
endif
	make clean

testbin:
	gcc -o testbin testbin.c
clean:
	rm -f *.o *.elf *.a testbin *.o64 *.elf64 *.lds
	make -C lib clean
	make -C libm clean
cleanall:
	rm -f *.bin* *.bin64* *.BIN *.BIN64
	make clean

include rules.make
