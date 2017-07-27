int mymain()
{
int p = 0x81000000;
int i;
for(i=0;i<0x4000000;i+=32)
{
 *(volatile int *)(p+i)=i;
 *(volatile int *)(0x82000000+i);
 if(*(volatile int *)(p+i) != i) printf("error verify\n");
 if((i&0xfffff) == 0) printf("%x\n",i);
}
return 0;
}
