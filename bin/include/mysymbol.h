#ifndef __MYSYMBOL_H__
#define __MYSYMBOL_H__
struct mysymbol{
char *name;
void *func;
};

#define mysymbol_export(func)                              \
	    struct mysymbol  __mysymbol_##func = {#func,func}; \
	    struct mysymbol  *__pmysymbol_##func __attribute__ ((unused,__section__ (".u_boot_cmd"))) = &__mysymbol_##func ;

extern char _start[];
extern struct mysymbol *__u_boot_cmd_start[], *__u_boot_cmd_end[];
static inline void * find_mysymbol(const char *name)
{
	struct mysymbol **psymbol;
	void *func;
	long reloc;
        struct mysymbol *p;

#ifdef __PIC__
	reloc = (long)_start;
#else
	reloc = 0;
#endif

	for(psymbol=__u_boot_cmd_start,func=0;psymbol<__u_boot_cmd_end;psymbol++)
	{
                p =  *psymbol;
                p = (void *)p + reloc;

		if(!strcmp(name,(void *)p->name+reloc))
		{
			func = (void *)p->func+reloc;
			break;
		}
	}
	return func;
}

#endif
