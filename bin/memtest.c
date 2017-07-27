int mymain()
{
 int i;
for(i=0;i<0x1000000;i+=4)
{
if((i&0xffff)==0)printf("w%08x\n",i);
 *(volatile int *)(0x80000000+i) = i;
}

for(i=0;i<0x1000000;i+=4)
{
if((i&0xffff)==0)printf("r%08x\n",i);
 if(*(volatile int *)(0x80000000+i) != i) printf("verify error on 0x%08x\n");
}
return 0;
}
