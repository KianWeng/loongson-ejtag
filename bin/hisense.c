unsigned long int rand_next = 1;

/*
 *  int rand()
 *      Taken from the K&R C programming language book. Page 46.
 *      returns a pseudo-random integer of 0..32767. Note that
 *      this is compatible with the System V function rand(), not
 *      with the bsd function rand() that returns 0..(2**31)-1.
 */
int 
rand ()
{
	rand_next = rand_next * 1103515245 + 12345;
	return ((unsigned int)(rand_next / 65536) % 32768);
}

/*
 *  srand(seed)
 *      companion routine to rand(). Initializes the seed.
 */
void
srand(unsigned int seed)
{
	rand_next = seed;
}

int initserial()
{

*(volatile int *)0xbf280004 = 0;
*(volatile int *)0xbf28000c = 0x1b;
*(volatile int *)0xbf28000c = 0x9f;
*(volatile int *)0xbf280040 = 0x67;
*(volatile int *)0xbf280020 = 0x5a;
*(volatile int *)0xbf280024 = 0;
*(volatile int *)0xbf28000c = 0x1b;
*(volatile int *)0xbf280008 = 0x87;
*(volatile int *)0xbf280014 = 0;
*(volatile int *)0xbf280000 = 0x42;
return 0;
}

int putchar(int c)
{
//*(volatile char *)SERIAL_REG=c;
while(!(*(volatile int *)0xbf280014&0x20));
*(volatile int *)0xbf280000=c;
return 0;
}

int mymain()
{
volatile char *p = 0x90000000;
int i=0,j=0,k=0;

initserial();

for(j=0;;j++)
{
if((j&0xff)==0)
 printf("%d\n", j);
		for(k=0;k<0x08000000;k+=0x100000)
                {
			for(i=0;i<0x1000;i=i+32)
			{
				p[k+i+0]=i;
				p[k+i+0x1000]=i;
				p[k+i+0x2000]=i;
				p[k+i+0x3000]=i;

			}
			p[rand()&0x07ffffff];
			p[rand()&0x07ffffff];
			p[rand()&0x07ffffff];
			p[rand()&0x07ffffff];

			for(i=0;i<0x1000;i=i+32)
				p[k+i+0x4000] = i;
			p[rand()&0x07ffffff];
		}
}

}

