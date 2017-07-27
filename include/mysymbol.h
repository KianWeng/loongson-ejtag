#ifndef __MYSYMBOL_H__
#define __MYSYMBOL_H__
struct mysymbol{
char *name;
void *func;
struct mysymbol *next;
};

#define mysymbol_export(func)                              \
	    struct mysymbol  __mysymbol_##func = {#func,func,0}; \
	    struct mysymbol  *__pmysymbol_##func __attribute__ ((unused,__section__ (".mysymbol"))) = &__mysymbol_##func ;

int strcmp(const char *s1, const char *s2);
static inline void *find_mysymbol(struct mysymbol *head,char *name)
{
 struct mysymbol *p;
 for(p=head;p;p=p->next)
  if(!strcmp(name,p->name)) break;
  return p?p->func:0;
}



extern struct mysymbol *mysymbol_head;
extern struct mysymbol *mysymbol_tail;
#endif
