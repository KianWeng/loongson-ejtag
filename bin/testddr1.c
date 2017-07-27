int mymain()
{
unsigned int p;
int i,j;
unsigned int d0,d1;
asm(".set mips32");

//for(i=0;i<10;i++)
// printf("done\n");
//asm("move $2,%0;cache 0x1c,($2);"::"r"(0x80000000):"$2");
//asm("move $2,%0;cache 0x1c,($2);"::"r"(0x80001000):"$2");
//asm("move $2,%0;cache 0x1c,($2);"::"r"(0x80002000):"$2");

for(p=0x80000000;p<0x86000000;p+=0x1000)
{
	int j;
	for(j=0;j<32;j=j+4)
	{
		*(volatile int *)(p+j)=(p+j);
	}
}

for(j=0;j<50;j++)
{
for(i=0;i<200;i++)
{
for(p=0x80000000;p<0x86000000;p+=0x4000)
{
 *(volatile int *)p=p;
 *(volatile int *)(p+0x1000)=p+0x1000;
 *(volatile int *)(p+0x2000)=p+0x2000;
asm("move $2,%0;cache 0x1,($2);cache 0x1,0x1000($2);cache 0x1,0x2000($2);"::"r"(p):"$2");
 *(volatile int *)(p+0x3000)=p+0x3000;
 if((d0=*(volatile int *)(p+0x1000)) != (p+0x1000)){
 printf("error verify %x!=%x,i=%d\n",d0,(p+0x1000),i);
goto done;
}
 //if((p&0x1fffff) == 0) printf("%x\n",p);
}
// printf("done\n");
}
printf("%d\n",j);
}
done:
 printf("done\n");
return 0;
}
