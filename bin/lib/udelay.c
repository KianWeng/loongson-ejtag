void udelay(int us)
{
int count0,count1;
int debug=0;
us *=CPU_COUNT_PER_US;
asm volatile("mfc0 %0,$23;li $2,0x2000000;or $2,%0;mtc0 $2,$23;":"=r"(debug)::"$2");
asm volatile("mfc0 %0,$9":"=r"(count0)); 
do{
asm volatile("mfc0 %0,$9":"=r"(count1)); 
}while(count1 -count0<us);
asm volatile("mtc0 %0,$23;"::"r"(debug));
}
