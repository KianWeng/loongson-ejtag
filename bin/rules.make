CROSS_COMPILE?=mipsel-linux-

CFLAGS =
ifdef HAVE_FLOAT
CFLAGS += -DHAVE_FLOAT -DFLOATINGPT
endif

CFLAGS1 := $(CFLAGS)  -O2 -g  -DGUEST -I include -I . -fno-pic -mno-abicalls -nostdinc -nostdlib -fno-builtin -mno-branch-likely -DCPU_COUNT_PER_US=10 -I $(TOPDIR)/include -include common.h

ifdef LOADADDR
ifndef NOFIXUP
CFLAGS += -Wa,-mfix-jrjalr-with-hb -Wa,-mfix-ls-jump=0xfffff -Wa,-mfix-ls-jump1=$(LOADADDR) -DFIXUP
endif

CFLAGS := $(CFLAGS) -O2 -g -DGUEST -I include -I . -fno-pic -mno-abicalls -nostdinc -nostdlib -fno-builtin -mno-branch-likely -DCPU_COUNT_PER_US=10 -I $(TOPDIR)/include -include common.h
affix=-$(LOADADDR)
else
CFLAGS := $(CFLAGS) -O2 -g -DGUEST -I include -I . -fpic -mabicalls -nostdinc -nostdlib -fno-builtin -mno-branch-likely -DCPU_COUNT_PER_US=10 -I $(TOPDIR)/include -include common.h
LOADADDR = 0
affix=
endif

export CFLAGS


libtinyc.a libtinyc64.a:
	make -C $(TOPDIR)/lib src=$(TOPDIR) $(obj)/lib $(obj)/$@
libm.a libm64.a:
	make -C $(TOPDIR)/libm src=$(TOPDIR) $(obj)/libm $(obj)/$@


.SUFFIXES: .elf .o64 .elf64 .bin64 .BIN .i .O .O64 .BIN .BIN64 .ELF .BIN64


%.elf:%.o start.o libtinyc.a libm.a bin.lds
	${CROSS_COMPILE}ld -g -T  bin.lds $(LDFLAGS) -o $@ start.o $<  -L . -ltinyc -lm


%.bin$(affix):%.elf
	${CROSS_COMPILE}objcopy -O binary $< $@ 

%.elf64:%.o64 start64.o64 libtinyc64.a libm.a bin64.lds
	${CROSS_COMPILE}ld -g -T  bin64.lds $(LDFLAGS) -o $@ start64.o64 $<  -L . -ltinyc64 -lm


%.bin64$(affix):%.elf64
	${CROSS_COMPILE}objcopy -O binary $< $@ 


%.ELF:%.O  bin.lds
	${CROSS_COMPILE}ld -g -T  bin.lds $(LDFLAGS) -o $@  $< 

%.BIN:%.ELF
	${CROSS_COMPILE}objcopy -O binary  $< $@$(affix)

%.ELF64:%.O64  bin64.lds
	${CROSS_COMPILE}ld -g -T  bin64.lds $(LDFLAGS) -o $@  $< 

%.BIN64:%.ELF64
	${CROSS_COMPILE}objcopy -O binary  $< $@$(affix)

bin.lds: $(TOPDIR)/bin.lds.S
	${CROSS_COMPILE}gcc -Umips -DLOADADDR=$(LOADADDR) -E -P -o $@ $(TOPDIR)/bin.lds.S 

bin64.lds: $(TOPDIR)/bin.lds.S
	${CROSS_COMPILE}gcc -Umips -DLOADADDR=$(LOADADDR) -E -P -o $@ $(TOPDIR)/bin64.lds.S

$(obj)/%.o %.o: $(src)/%.S
	${CROSS_COMPILE}gcc  $(CFLAGS) -o $@ -c $< 
$(obj)/%.o %.o: $(src)/%.c
	${CROSS_COMPILE}gcc  $(CFLAGS) -o $@ -c $< 
$(obj)/%.o64 %.o64: $(src)/%.S
	${CROSS_COMPILE}gcc  $(CFLAGS) -o $@ -c $<  -mabi=64 -DHAVE_QUAD
$(obj)/%.o64 %.o64: $(src)/%.c
	${CROSS_COMPILE}gcc  $(CFLAGS) -o $@ -c $<  -mabi=64 -DHAVE_QUAD
$(obj)/%.s %.s: $(src)/%.S
	${CROSS_COMPILE}gcc  $(CFLAGS) -S -fverbose-asm -o $@ $< 
$(obj)/%.s %.s: $(src)/%.c
	${CROSS_COMPILE}gcc  $(CFLAGS) -S -fverbose-asm -o $@  $< 
$(obj)/%.s64 %.s64: $(src)/%.S
	${CROSS_COMPILE}gcc  $(CFLAGS) -S -fverbose-asm -o $@ -c $<  -mabi=64 -DHAVE_QUAD
$(obj)/%.s64 %.s64: $(src)/%.c
	${CROSS_COMPILE}gcc  $(CFLAGS) -S -fverbose-asm -o $@ -c $<  -mabi=64 -DHAVE_QUAD
$(obj)/%.O %.O: $(src)/%.S
	${CROSS_COMPILE}gcc  $(CFLAGS1) -o $@ -c $< 
$(obj)/%.O %.O: $(src)/%.c
	${CROSS_COMPILE}gcc  $(CFLAGS1) -o $@ -c $< 
$(obj)/%.O64 %.O64: $(src)/%.S
	${CROSS_COMPILE}gcc  $(CFLAGS1) -o $@ -c $<  -mabi=64 -DHAVE_QUAD
$(obj)/%.O64 %.O64: $(src)/%.c
	${CROSS_COMPILE}gcc  $(CFLAGS1) -o $@ -c $<  -mabi=64 -DHAVE_QUAD
