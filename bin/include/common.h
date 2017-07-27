#include <../../include/regdef.h>
#define SERIAL_REG (0xffffffffff200000+SERIAL)
#define RTC_REG    (0xffffffffff200000+RTC)
#define ARGC_REG (0xffffffffff200000+RET_ADDR)
#define HEX_REG  (0xffffffffff200000+HEX_ADDR)

#define ___constant_swab16(x) ((unsigned short)(				\
	(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) |			\
	(((unsigned short)(x) & (unsigned short)0xff00U) >> 8)))

#define ___constant_swab32(x) ((unsigned int)(				\
	(((unsigned int)(x) & (unsigned int)0x000000ffUL) << 24) |		\
	(((unsigned int)(x) & (unsigned int)0x0000ff00UL) <<  8) |		\
	(((unsigned int)(x) & (unsigned int)0x00ff0000UL) >>  8) |		\
	(((unsigned int)(x) & (unsigned int)0xff000000UL) >> 24)))

#define ___constant_swab64(x) ((unsigned long long)(				\
	(((unsigned long long)(x) & (unsigned long long)0x00000000000000ffULL) << 56) |	\
	(((unsigned long long)(x) & (unsigned long long)0x000000000000ff00ULL) << 40) |	\
	(((unsigned long long)(x) & (unsigned long long)0x0000000000ff0000ULL) << 24) |	\
	(((unsigned long long)(x) & (unsigned long long)0x00000000ff000000ULL) <<  8) |	\
	(((unsigned long long)(x) & (unsigned long long)0x000000ff00000000ULL) >>  8) |	\
	(((unsigned long long)(x) & (unsigned long long)0x0000ff0000000000ULL) >> 24) |	\
	(((unsigned long long)(x) & (unsigned long long)0x00ff000000000000ULL) >> 40) |	\
	(((unsigned long long)(x) & (unsigned long long)0xff00000000000000ULL) >> 56)))

#ifndef __ASSEMBLER__
typedef void * va_list;
#if __GNUC__<3 || (__GNUC__==3 && __GNUC_MINOR__<3)
#define va_start(ap,fmt) ap=((void *)&fmt)+1;
#else
#define va_start __builtin_va_start
#endif
#define va_arg __builtin_va_arg
#define va_end __builtin_va_end
#define __P(x) x
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned int uint32_t;
typedef unsigned int size_t;
typedef long long quad_t;
typedef	unsigned long long u_quad_t;	/* quads */


int printf (const char *fmt, ...);
int newprintf (const char *fmt, ...);
int printbase(long v,int w,int base,int sign);
int snprintbase(char *d, int n, long v,int w,int base,int sign);
int sprintbase(char *d, long v,int w,int base,int sign);
int tvsnprintf(char *buf, int n, const char *fmt, void **arg);
int tsnprintf(char *buf, int n, const char *fmt,...);
int tsprintf(char *buf, const char *fmt,...);
#define isdigit(c) (c>='0' && c<='9')
#define FMT_RJUST 0
#define FMT_LJUST 1
#define FMT_RJUST0 2
#define FMT_CENTER 3

#define NULL ((void *)0)

#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0\n\t"				\
			: "=r" (__res));				\
	__res;								\
})


#define __read_64bit_c0_split(source, sel)				\
({									\
	unsigned long long val;						\
	unsigned long flags;						\
									\
	local_irq_save(flags);						\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmfc0\t%M0, " #source "\n\t"			\
			"dsll\t%L0, %M0, 32\n\t"			\
			"dsrl\t%M0, %M0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			".set\tmips0"					\
			: "=r" (val));					\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmfc0\t%M0, " #source ", " #sel "\n\t"		\
			"dsll\t%L0, %M0, 32\n\t"			\
			"dsrl\t%M0, %M0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			".set\tmips0"					\
			: "=r" (val));					\
	local_irq_restore(flags);					\
									\
	val;								\
})

#define __read_64bit_c0_register(source, sel)				\
({ unsigned long long __res;						\
	if (sizeof(unsigned long) == 4)					\
		__res = __read_64bit_c0_split(source, sel);		\
	else if (sel == 0)						\
		__asm__ __volatile__(					\
			".set\tmips3\n\t"				\
			"dmfc0\t%0, " #source "\n\t"			\
			".set\tmips0"					\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0"					\
			: "=r" (__res));				\
	__res;								\
})



#define __write_32bit_c0_register(register, sel, value)			\
do {									\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register "\n\t"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)


#define __write_64bit_c0_split(source, sel, val)			\
do {									\
	unsigned long flags;						\
									\
	local_irq_save(flags);						\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dsll\t%L0, %L0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			"dsll\t%M0, %M0, 32\n\t"			\
			"or\t%L0, %L0, %M0\n\t"				\
			"dmtc0\t%L0, " #source "\n\t"			\
			".set\tmips0"					\
			: : "r" (val));					\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dsll\t%L0, %L0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			"dsll\t%M0, %M0, 32\n\t"			\
			"or\t%L0, %L0, %M0\n\t"				\
			"dmtc0\t%L0, " #source ", " #sel "\n\t"		\
			".set\tmips0"					\
			: : "r" (val));					\
	local_irq_restore(flags);					\
} while (0)

#define __write_64bit_c0_register(register, sel, value)			\
do {									\
	if (sizeof(unsigned long) == 4)					\
		__write_64bit_c0_split(register, sel, value);		\
	else if (sel == 0)						\
		__asm__ __volatile__(					\
			".set\tmips3\n\t"				\
			"dmtc0\t%z0, " #register "\n\t"			\
			".set\tmips0"					\
			: : "Jr" (value));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" (value));				\
} while (0)


#define __read_ulong_c0_register(reg, sel)				\
	((sizeof(unsigned long) == 4) ?					\
	(unsigned long) __read_32bit_c0_register(reg, sel) :		\
	(unsigned long) __read_64bit_c0_register(reg, sel))

#define __write_ulong_c0_register(reg, sel, val)			\
do {									\
	if (sizeof(unsigned long) == 4)					\
		__write_32bit_c0_register(reg, sel, val);		\
	else								\
		__write_64bit_c0_register(reg, sel, val);		\
} while (0)



#define read_c0_index()		__read_32bit_c0_register($0, 0)
#define write_c0_index(val)	__write_32bit_c0_register($0, 0, val)

#define read_c0_entrylo0()	__read_ulong_c0_register($2, 0)
#define write_c0_entrylo0(val)	__write_ulong_c0_register($2, 0, val)

#define read_c0_entrylo1()	__read_ulong_c0_register($3, 0)
#define write_c0_entrylo1(val)	__write_ulong_c0_register($3, 0, val)

#define read_c0_conf()		__read_32bit_c0_register($3, 0)
#define write_c0_conf(val)	__write_32bit_c0_register($3, 0, val)

#define read_c0_context()	__read_ulong_c0_register($4, 0)
#define write_c0_context(val)	__write_ulong_c0_register($4, 0, val)

#define read_c0_pagemask()	__read_32bit_c0_register($5, 0)
#define write_c0_pagemask(val)	__write_32bit_c0_register($5, 0, val)

#define read_c0_wired()		__read_32bit_c0_register($6, 0)
#define write_c0_wired(val)	__write_32bit_c0_register($6, 0, val)

#define read_c0_info()		__read_32bit_c0_register($7, 0)

#define read_c0_cache()		__read_32bit_c0_register($7, 0)	/* TX39xx */
#define write_c0_cache(val)	__write_32bit_c0_register($7, 0, val)

#define read_c0_badvaddr()	__read_ulong_c0_register($8, 0)
#define write_c0_badvaddr(val)	__write_ulong_c0_register($8, 0, val)

#define read_c0_count()		__read_32bit_c0_register($9, 0)
#define write_c0_count(val)	__write_32bit_c0_register($9, 0, val)

#define read_c0_count2()	__read_32bit_c0_register($9, 6) /* pnx8550 */
#define write_c0_count2(val)	__write_32bit_c0_register($9, 6, val)

#define read_c0_count3()	__read_32bit_c0_register($9, 7) /* pnx8550 */
#define write_c0_count3(val)	__write_32bit_c0_register($9, 7, val)

#define read_c0_entryhi()	__read_ulong_c0_register($10, 0)
#define write_c0_entryhi(val)	__write_ulong_c0_register($10, 0, val)

#define read_c0_compare()	__read_32bit_c0_register($11, 0)
#define write_c0_compare(val)	__write_32bit_c0_register($11, 0, val)

#define read_c0_compare2()	__read_32bit_c0_register($11, 6) /* pnx8550 */
#define write_c0_compare2(val)	__write_32bit_c0_register($11, 6, val)

#define read_c0_compare3()	__read_32bit_c0_register($11, 7) /* pnx8550 */
#define write_c0_compare3(val)	__write_32bit_c0_register($11, 7, val)

#define read_c0_status()	__read_32bit_c0_register($12, 0)

#define write_c0_status(val)	__write_32bit_c0_register($12, 0, val)


#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_cause(val)	__write_32bit_c0_register($13, 0, val)

#define read_c0_epc()		__read_ulong_c0_register($14, 0)
#define write_c0_epc(val)	__write_ulong_c0_register($14, 0, val)

#define read_c0_prid()		__read_32bit_c0_register($15, 0)

#define read_c0_config()	__read_32bit_c0_register($16, 0)
#define read_c0_config1()	__read_32bit_c0_register($16, 1)
#define read_c0_config2()	__read_32bit_c0_register($16, 2)
#define read_c0_config3()	__read_32bit_c0_register($16, 3)
#define read_c0_config4()	__read_32bit_c0_register($16, 4)
#define read_c0_config5()	__read_32bit_c0_register($16, 5)
#define read_c0_config6()	__read_32bit_c0_register($16, 6)
#define read_c0_config7()	__read_32bit_c0_register($16, 7)
#define write_c0_config(val)	__write_32bit_c0_register($16, 0, val)
#define write_c0_config1(val)	__write_32bit_c0_register($16, 1, val)
#define write_c0_config2(val)	__write_32bit_c0_register($16, 2, val)
#define write_c0_config3(val)	__write_32bit_c0_register($16, 3, val)
#define write_c0_config4(val)	__write_32bit_c0_register($16, 4, val)
#define write_c0_config5(val)	__write_32bit_c0_register($16, 5, val)
#define write_c0_config6(val)	__write_32bit_c0_register($16, 6, val)
#define write_c0_config7(val)	__write_32bit_c0_register($16, 7, val)

/*
 * The WatchLo register.  There may be upto 8 of them.
 */
#define read_c0_watchlo0()	__read_ulong_c0_register($18, 0)
#define read_c0_watchlo1()	__read_ulong_c0_register($18, 1)
#define read_c0_watchlo2()	__read_ulong_c0_register($18, 2)
#define read_c0_watchlo3()	__read_ulong_c0_register($18, 3)
#define read_c0_watchlo4()	__read_ulong_c0_register($18, 4)
#define read_c0_watchlo5()	__read_ulong_c0_register($18, 5)
#define read_c0_watchlo6()	__read_ulong_c0_register($18, 6)
#define read_c0_watchlo7()	__read_ulong_c0_register($18, 7)
#define write_c0_watchlo0(val)	__write_ulong_c0_register($18, 0, val)
#define write_c0_watchlo1(val)	__write_ulong_c0_register($18, 1, val)
#define write_c0_watchlo2(val)	__write_ulong_c0_register($18, 2, val)
#define write_c0_watchlo3(val)	__write_ulong_c0_register($18, 3, val)
#define write_c0_watchlo4(val)	__write_ulong_c0_register($18, 4, val)
#define write_c0_watchlo5(val)	__write_ulong_c0_register($18, 5, val)
#define write_c0_watchlo6(val)	__write_ulong_c0_register($18, 6, val)
#define write_c0_watchlo7(val)	__write_ulong_c0_register($18, 7, val)

/*
 * The WatchHi register.  There may be upto 8 of them.
 */
#define read_c0_watchhi0()	__read_32bit_c0_register($19, 0)
#define read_c0_watchhi1()	__read_32bit_c0_register($19, 1)
#define read_c0_watchhi2()	__read_32bit_c0_register($19, 2)
#define read_c0_watchhi3()	__read_32bit_c0_register($19, 3)
#define read_c0_watchhi4()	__read_32bit_c0_register($19, 4)
#define read_c0_watchhi5()	__read_32bit_c0_register($19, 5)
#define read_c0_watchhi6()	__read_32bit_c0_register($19, 6)
#define read_c0_watchhi7()	__read_32bit_c0_register($19, 7)

#define write_c0_watchhi0(val)	__write_32bit_c0_register($19, 0, val)
#define write_c0_watchhi1(val)	__write_32bit_c0_register($19, 1, val)
#define write_c0_watchhi2(val)	__write_32bit_c0_register($19, 2, val)
#define write_c0_watchhi3(val)	__write_32bit_c0_register($19, 3, val)
#define write_c0_watchhi4(val)	__write_32bit_c0_register($19, 4, val)
#define write_c0_watchhi5(val)	__write_32bit_c0_register($19, 5, val)
#define write_c0_watchhi6(val)	__write_32bit_c0_register($19, 6, val)
#define write_c0_watchhi7(val)	__write_32bit_c0_register($19, 7, val)


static inline void tlb_write_indexed(void)
{
	__asm__ __volatile__(
		".set noreorder\n\t"
		"tlbwi\n\t"
		".set reorder");
}
#endif
