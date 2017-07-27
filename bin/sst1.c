#define	SST_CMDOFFS	0x5555
#define SST_CMDOFFS1 0x5555
#define SST_CMDOFFS2 0x2aaa
#define FL_ERASE	0x80
#define FL_ERASE_CHIP	0x10
#define FL_PROGRAM 0xa0
#define FL_AUTOSEL 	0x90	/* Device identification */
#define FL_SECT		0x30
#define FL_BLOCK		0x50

int off[]={0,0x5555,0x2aaa};
long  base=0xffffffffbfc08000UL;
int sectorsize=0x1000;
unsigned char fl_id=0x5a,fl_mfg=0xbf;

#define outb(a,v) *(volatile char *)((int)a) = (v);
#define outl(a,v) *(volatile int *)((int)a) = (v);
#define inl(a) (*(volatile int *)((int)a))

int common_op(char *c,int size)
{
int i;
for(i=0;i<size;i+=2)
{
//printf("w %02x -> %04x\n", (unsigned char)c[i+1],off[c[i]]);
*(volatile char *)(base+off[c[i]]) = c[i+1];
}
return 0;
}

int reset()
{
unsigned char c[]={0,0xf0};
common_op(c,sizeof(c));
return 0;
}


	int unlock()
{
	unsigned int trans_unlock_value;
	unsigned int value;

	printf("mfg=0x%x,id=0x%x\n",fl_mfg,fl_id);

	if (!((((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x50))   //NOT SST49LF040B
				||(((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x5a)))) /* NOT SST49LF008A */
		return(0);

	if (((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x50)) /* SST49LF040B */
	{
		printf("Disable all space write protection of 49LF040B. \r\n");
		/* Open translation of 0xbc000000 - 0xbd00000 */
		trans_unlock_value = inl(0xbfe00200);
		outl(0xbfe00200, (0x00ff0000 | trans_unlock_value));

		/* Disable all space write protection */
		outb(0xbdbf0002, 0x0);
		outb(0xbdbe0002, 0x0);
		outb(0xbdbd0002, 0x0);
		outb(0xbdbc0002, 0x0);
		outb(0xbdbb0002, 0x0);
		outb(0xbdba0002, 0x0);
		outb(0xbdb90002, 0x0);
		outb(0xbdb80002, 0x0);

		outl(0xbfe00200, trans_unlock_value);
	}
	else if (((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x5a))   /* SST49LF008A */
	{
		printf("Disable all space write protection of 49LF008A. \r\n");
		/* Open translation of 0xbc000000 - 0xbd00000 */
		trans_unlock_value = inl(0xbfe00200);
		outl(0xbfe00200, (0x00870000 | trans_unlock_value));
		/* Enable firmware memory access */
		value = inl(0xbfe00204);
		value |= 1 << 31;
		outl(0xbfe00204, value);

		/* Disable all space write protection */
		outb(0xbdbf0002, 0x0);
		outb(0xbdbe0002, 0x0);
		outb(0xbdbd0002, 0x0);
		outb(0xbdbc0002, 0x0);
		outb(0xbdbb0002, 0x0);
		outb(0xbdba0002, 0x0);
		outb(0xbdb90002, 0x0);
		outb(0xbdb80002, 0x0);
		outb(0xbdb70002, 0x0);
		outb(0xbdb60002, 0x0);
		outb(0xbdb50002, 0x0);
		outb(0xbdb40002, 0x0);
		outb(0xbdb30002, 0x0);
		outb(0xbdb20002, 0x0);
		outb(0xbdb10002, 0x0);
		outb(0xbdb00002, 0x0);

		outl(0xbfe00200, trans_unlock_value);
	}
	return(1);
}

/*
 *  Function to enable the LPC write protection of SST49LF040B/enable the FIRMWARE HUB write protection of SST49LF008A
 *  For Loongson3 by wanghuandong(AdonWang, whd)
 */

	int lock()
{
	unsigned int trans_unlock_value;
	unsigned int value;

	if (!((((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x50))   /* NOT SST49LF040B */
				||(((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x5a)))) /* NOT SST49LF008A */
		return(0);

	if (((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x50)) /* SST49LF040B */
	{
		printf("Enable all space write protection of 49LF040B. \r\n");
		/* Open translation of 0xbc000000 - 0xbd00000 */
		trans_unlock_value = inl(0xbfe00200);
		outl(0xbfe00200, (0x00ff0000 | trans_unlock_value));

		/* Enable all space write protection */
		outb(0xbdbf0002, 0x1);
		outb(0xbdbe0002, 0x1);
		outb(0xbdbd0002, 0x1);
		outb(0xbdbc0002, 0x1);
		outb(0xbdbb0002, 0x1);
		outb(0xbdba0002, 0x1);
		outb(0xbdb90002, 0x1);
		outb(0xbdb80002, 0x1);

		outl(0xbfe00200, trans_unlock_value);
	}
	else if (((fl_mfg & 0xff) == 0xbf) && ((fl_id & 0xff) == 0x5a))   /* SST49LF008A */
	{
		printf("Enable all space write protection of 49LF008A. \r\n");
		/* Open translation of 0xbc000000 - 0xbd00000 */
		trans_unlock_value = inl(0xbfe00200);
		outl(0xbfe00200, (0x00870000 | trans_unlock_value));
		/* Enable firmware memory access */
		value = inl(0xbfe00204);
		value |= 1 << 31;
		outl(0xbfe00204, value);

		/* Enable all space write protection */
		outb(0xbdbf0002, 0x1);
		outb(0xbdbe0002, 0x1);
		outb(0xbdbd0002, 0x1);
		outb(0xbdbc0002, 0x1);
		outb(0xbdbb0002, 0x1);
		outb(0xbdba0002, 0x1);
		outb(0xbdb90002, 0x1);
		outb(0xbdb80002, 0x1);
		outb(0xbdb70002, 0x1);
		outb(0xbdb60002, 0x1);
		outb(0xbdb50002, 0x1);
		outb(0xbdb40002, 0x1);
		outb(0xbdb30002, 0x1);
		outb(0xbdb20002, 0x1);
		outb(0xbdb10002, 0x1);
		outb(0xbdb00002, 0x1);

		outl(0xbfe00200, trans_unlock_value);
	}
	return(1);
}


int identify()
{
unsigned char c[]={1,0xaa,2,0x55,1,FL_AUTOSEL};
common_op(c,sizeof(c));
printf("mfg=%x,id=%x\n",((volatile unsigned char *)base)[0],((volatile unsigned char *)base)[1]);
fl_mfg = ((volatile unsigned char *)base)[0];
fl_id = ((volatile unsigned char *)base)[1];
reset();
return 0;
}

int erase_chip()
{
char c[]={1,0xaa,2,0x55,1,FL_ERASE,1,0xaa,2,0x55,1,FL_ERASE_CHIP};
int i;
printf("erase chip\n");
common_op(c,sizeof(c));
while( *(volatile char *)(base+0)!= *(volatile char *)(base+0));
return 0;
}


int erase_sect(int soff,int eoff,int sectorsize)
{
char c[]={1,0xaa,2,0x55,1,FL_ERASE,1,0xaa,2,0x55,0,FL_SECT};
int i;
int old;
printf("erase sect\n");
for(i=soff;i<eoff;i+=sectorsize)
{
off[0] = i;
common_op(c,sizeof(c));
while( *(volatile char *)(base+i)!= *(volatile char *)(base+i));
}
printf("erase done\n");
return 0;
}

int program(char *buf,int soff,int eoff)
{
char c[]={1,0xaa,2,0x55,1,FL_PROGRAM};
int i;
printf("program\n");
for(i=soff;i<eoff;i++)
{
if((i&0xfff)==0)printf("%x\n",i);
	common_op(c,sizeof(c));
	*(volatile char *)(base+i) = buf[i-soff];
	while( *(volatile char *)(base+i)!= *(volatile char *)(base+i));
}
printf("%x\n",i);
printf("program done\n");
return 0;
}

#define HEX_REG (0xffffffffff200000+HEX_ADDR)
int mymain(char *cmd,unsigned long flashbase,...)
{
int i;
register long sp asm("$29");
for(i=0;i<10;i++)
*(volatile int *)HEX_REG = i;
*(volatile long *)HEX_REG= sp;
//*(volatile long *)HEX_REG= printf;
*(volatile int *)HEX_REG = 10;
printf("test\r\n");
return 0;
}

