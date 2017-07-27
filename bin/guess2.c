#ifdef HAVE_FLOAT
#include <math.h>
#define printf newprintf
#endif

#ifdef _ABIO32
#define LADDU "addu"
#else
#define LADDU "daddu"
#endif

static unsigned long GUESS_ADDR = ((int)0xbfc00000);
static unsigned long mask = 0xffff;
static unsigned long stride = 4;

int scan()
{
	void *p;
	for(p=0xffffffff90000000;p<0xffffffffa0000000;p=p+4)
	{
		if((((long)p)&0xffff)==0) printf("%x\n", p);
		*(volatile int *)p;
	}

}


int guess(unsigned long addr, unsigned long end)
{
	*(int *)SERIAL_REG =  ___constant_swab32('g \r\n');
	while(addr<end)
	{
		*(long *)HEX_REG = addr;
		asm volatile(
				".set push;\n"
				".set noreorder;\n"
				".set mips64;\n"
				"1:lw %1,(%2);\n"
				"beqzl  %1,1f;\n"
				"nop;\n"
				"sync;\n"
				LADDU " %0, %5;\n"
				"and %1, %0, %4;\n"
				"beqz %1,2f;\n"
				"nop;\n"
				"b 1b;\n"
				"nop;\n"
				"1:lw %1,(%0);\n"
				"b 1b;\n"
				"nop;\n"
				"2:\n"
				".set pop;\n"
				:"=r"(addr):"r"(0),"r"(GUESS_ADDR),"0"(addr),"r"(mask), "r"(stride)
			    );
	}
}


int guess1(unsigned long addr, unsigned long end)
{
	*(int *)SERIAL_REG =  ___constant_swab32('G \r\n');
	while(addr<end)
	{
		*(long *)HEX_REG = addr;
		asm volatile(
				".set push;\n"
				".set noreorder;\n"
				".set mips64;\n"
				"1:lw %1,(%2);\n"
				"beqzl  %1,1f;\n"
				"nop;\n"
				"sync;\n"
				LADDU " %0,%5;\n"
				"and %1, %0, %4;\n"
				"beqz %1,2f;\n"
				"nop;\n"
				"b 1b;\n"
				"nop;\n"
				".set fix_ls_jump=0;\n"
				"1:jr %0;\n"
				".set fix_ls_jump=0xffff;\n"
				"nop;\n"
				"2:\n"
				".set pop;\n"
				:"=r"(addr):"r"(0),"r"(GUESS_ADDR),"0"(addr),"r"(mask),"r"(stride)
			    );
	}
}


int mymain(long from,long to, long maskval, long strideval, int modeval)
{
	int argc = *(int *)ARGC_REG;
	int mode = 3;
	while(*(volatile int *)GUESS_ADDR == 0)
        GUESS_ADDR += 4;
	printf("guess addr %x\n",GUESS_ADDR);
	printf("begin test\n");

//	scan();
        
        if(argc>2) mask = maskval;
        if(argc>3) stride = strideval;
        if(argc>4) mode=modeval;

	if(argc>=2)
	{
        if(mode&1)
	guess1(from, to);
        if(mode&2)
	guess(from, to);
	}
	else
	{
	guess1(0xffffffffbf000000, 0xffffffffc0000000);
	guess(0xffffffffbf000000, 0xffffffffc0000000);
	guess1(0xffffffff9f000000, 0xffffffffa0000000);
	guess(0xffffffff9f000000, 0xffffffffa0000000);
	}
	return 0;
}

