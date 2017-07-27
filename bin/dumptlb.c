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

void mymain(int ntlb)
{
	unsigned long s_entryhi, entryhi, entrylo0, entrylo1, asid;
	unsigned int s_index, pagemask, c0, c1, i;

	ntlb = *(int *)ARGC_REG?ntlb:32;

	s_entryhi = read_c0_entryhi();
	s_index = read_c0_index();
	asid = s_entryhi & 0xff;

	for (i = 0; i < ntlb; i++) {
		write_c0_index(i);

	__asm__ __volatile__(
		".set noreorder\n\t"
		"tlbr\n\t"
		".set reorder");

		pagemask = read_c0_pagemask();
		entryhi  = read_c0_entryhi();
		entrylo0 = read_c0_entrylo0();
		entrylo1 = read_c0_entrylo1();

			/*
			 * Only print entries in use
			 */
			printf("Index: %d pgmask=%s hi=0x%x lo0=0x%08x lo1=0x%08x ", i, msk2str(pagemask), entryhi, entrylo0, entrylo1);

			c0 = (entrylo0 >> 3) & 7;
			c1 = (entrylo1 >> 3) & 7;

			printf("va=%x asid=%x\n",
			       (entryhi & ~0x1fffUL),
			       entryhi & 0xff);
			printf("\t[pa=%x c=%d d=%d v=%d g=%d] ",
			       (entrylo0 << 6) & PAGE_MASK, c0,
			       (entrylo0 & 4) ? 1 : 0,
			       (entrylo0 & 2) ? 1 : 0,
			       (entrylo0 & 1));
			printf("[pa=%x c=%d d=%d v=%d g=%d]\n",
			       (entrylo1 << 6) & PAGE_MASK, c1,
			       (entrylo1 & 4) ? 1 : 0,
			       (entrylo1 & 2) ? 1 : 0,
			       (entrylo1 & 1));
	}
	printf("\n");

	write_c0_entryhi(s_entryhi);
	write_c0_index(s_index);
}

