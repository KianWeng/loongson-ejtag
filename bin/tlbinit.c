
#define ST0_ERL			0x00000004




#define PM_4K		0x00000000
#define PM_16K		0x00006000
#define PM_64K		0x0001e000
#define PM_256K		0x0007e000
#define PM_1M		0x001fe000
#define PM_4M		0x007fe000
#define PM_16M		0x01ffe000
#define PM_64M		0x07ffe000
#define PM_256M		0x1fffe000


static inline const char *msk2str(unsigned int mask)
{
	switch (mask) {
	case PM_4K:	return "4kb";
	case PM_16K:	return "16kb";
	case PM_64K:	return "64kb";
	case PM_256K:	return "256kb";
#ifndef CONFIG_CPU_VR41XX
	case PM_1M:	return "1Mb";
	case PM_4M:	return "4Mb";
	case PM_16M:	return "16Mb";
	case PM_64M:	return "64Mb";
	case PM_256M:	return "256Mb";
#endif
	}
	return "";
}


#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK       (~((1 << PAGE_SHIFT) - 1))

#define ASID_MASK	0xff


int tlb_init(int tlbs, int tlbe,int cachetype,unsigned int virtaddr, unsigned int phyaddr)
{
	int pid;
	int eflag=0;
	int i;
	


	pid = read_c0_entryhi() & ASID_MASK;
	
for(i=tlbs;i<tlbe;i++)
{
	write_c0_index(i);
	write_c0_pagemask(PM_16M);
	write_c0_entryhi(virtaddr | (pid));
	write_c0_entrylo0((phyaddr >> 6)|cachetype); //uncached,global
	write_c0_entrylo1(((phyaddr+(16<<20)) >> 6)|cachetype);
	tlb_write_indexed();
	virtaddr += 32<<20;
	phyaddr += 32<<20;
	if(virtaddr>=0x80000000)break;
}

return 0;
}

void mymain(unsigned int virtaddr, unsigned int phyaddr,int cache)
{
int i;
printf("virtaddr=0x%x, phyaddr=0x%x, cache=0x%x,vaddr|0x20000000 is uncached. 224M.\n",virtaddr, phyaddr,cache);

write_c0_status(read_c0_status()&~ST0_ERL);

for(i=0;i<32;i++)
tlb_init(i,i+1,0,0x80000000,0);
tlb_init(0,7,(cache<<3)|7,virtaddr, phyaddr);
tlb_init(8,15,(2<<3)|7,virtaddr|0x20000000, phyaddr);
}

