#ifndef __MYCONFIG_H__
#define __MYCONFIG_H__
struct myconfig{
char *name;
void *config;
int type;
char *note;
char *(*complete)(const char *text,int state);
struct myconfig *next;
};
#define CONFIG_I 1
#define CONFIG_S 2
#define CONFIG_LL 4
#define CONFIG_P  8


#define myconfig_init(name,config,type,note)                              \
	    struct myconfig  __myconfig_##config  = {name,&config,type,note,0,0}; \
 	    struct myconfig *__pmyconfig_##config __attribute__ ((unused,__section__ (".myconfig"))) = &__myconfig_##config ;
#define myconfig_init1(name,config,type,note,complete)                              \
	    struct myconfig  __myconfig_##config = {name,&config,type,note,complete,0}; \
	    struct myconfig  *__pmyconfig_##config __attribute__ ((unused,__section__ (".myconfig"))) = &__myconfig_##config ;
#define myconfig_alias(name,alias,config,type,note)                              \
	    struct myconfig  __myconfig_##alias  = {name,&config,type,note,0,0}; \
 	    struct myconfig *__pmyconfig_##alias __attribute__ ((unused,__section__ (".myconfig"))) = &__myconfig_##alias ;
#endif
