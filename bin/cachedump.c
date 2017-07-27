int type;
#define printf newprintf

int dumpcache()
{
unsigned long taglo;
int way;
unsigned long long phytag;
unsigned long addr=0xffffffff9fc00000;
long len=0x4000;

for(;len>0;len-=32,addr+=32)
{
way=(addr>>12)&3;

if(type==1)
{
asm(".set mips32;cache %2,(%1);mfc0 %0,$28;.set mips0;":"=r"(taglo):"r"(addr),"i"(5));
 phytag= (((unsigned long long)taglo>>8)&0xfffff)<<12;
 if(taglo)
 printf("phyaddr=0x%08lx,way=%d,stat=%ld,cs=%08lx\n",(long)phytag,way,(taglo>>6)&3,(taglo>>4)&3);
 }
else if(type==0)
{
asm(".set mips32;cache %2,(%1);mfc0 %0,$28;.set mips0;":"=r"(taglo):"r"(addr),"i"(4));
 phytag= (((unsigned long long)taglo>>8)&0xfffff)<<12;
 printf("phyaddr=0x%08lx,way=%d,lock=%ld,cs=%08lx\n",(long)phytag,way,(taglo>>28)&1,(taglo>>6)&3);
 }

}

return 0;
}

int mymain(int a0)
{
type = a0;
dumpcache();
}
