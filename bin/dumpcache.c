
int mymain(int addr,int len)
{
unsigned long taglo,taghi;
int way=0;
unsigned long long phytag;

for(;len>0;len-=32,addr+=32)
{

// for(way=0;way<=3;way++)
 {
  
asm(".set mips32;cache %3,(%2);mfc0 %0,$28;mfc0 %1,$29;.set mips0;":"=r"(taglo),"=r"(taghi):"r"(addr|way),"i"(5));
 phytag= ((((unsigned long long)taglo>>8)&0xffffff)<<12)|((((unsigned long long)taghi&0xf))<<36);
 printf("\n%x:state=%d,phyaddr=%x,way=%d,mode=%d,taglo=%x,taghi=%x\n",addr|way,(taglo>>6)&3,phytag,(taglo>>4)&3,(taghi>>29)&7,taglo,taghi);
}

}

return 0;
}
