int mymain(unsigned int base)
{
unsigned int p;
int i,error=0;
unsigned int d0,d1;
asm(".set mips32");
base = (base&0x1fffffff)|0x80000000;
printf("base=%x\n",base);

//for(i=0;i<10;i++)
// printf("done\n");
//asm("move $2,%0;cache 0x1c,($2);"::"r"(0x80000000):"$2");
//asm("move $2,%0;cache 0x1c,($2);"::"r"(0x80001000):"$2");
//asm("move $2,%0;cache 0x1c,($2);"::"r"(0x80002000):"$2");
#if 0
{
extern int code_start[],code_end[];
printf("start=%x,end=%x\n",code_start,code_end);
for(p=code_start;p<code_end;p+=32)
{
asm("cache 0x1c,(%0);"::"r"(p));
}

}
#endif

for(p=base;p<base+0x4000;p+=4)
{
		*(volatile int *)p=p;
}

asm(".align 5;.global code_start;code_start:");
for(i=0;i<50000;i++)
{
for(p=base;p<base+0x4000;p+=32)
{
 *(volatile int *)p=p;
}
for(p=base;p<base+0x1000;p+=32)
{
asm("cache 0x1,(%0);cache 0x1,0x1000(%0);cache 0x1,0x2000(%0);cache 0x1,0x3000(%0);"::"r"(p));
}
for(p=base;p<base+0x4000;p+=32)
{
 *(volatile int *)p;
}
for(p=base;p<base+0x4000;p+=32)
{
 if((d0=*(volatile int *)p) != p){
 printf("error verify %x!=%x,i=%d\n",d0,p,i);
error=1;
}
}
 if(error)
 goto done;
}
asm(".align 5;.global code_end;code_end:");
done:
 printf("done\n");
#if 0
{
extern int code_start[],code_end[];
for(p=code_start;p<code_end;p+=32)
{
asm("cache 0x10,(%0);"::"r"(p));
}

}
#endif
return 0;
}
