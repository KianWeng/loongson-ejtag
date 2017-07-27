#define inl(x) (*(volatile int *)(x))
#define REG_TOY_READ0 0xbfe6402c
#define REG_TOY_CNTRCTL 0xbfe64040

int mymain()
{
	int i;
	int cnt,cur;
	int timeout;
	int v,sec;
	int md_pipefreq;
	int config;

	/*enable cp0 count inc on ejtag mode*/

	asm volatile("mfc0 %0,$23;li $2,0x2000000;or $2,%0;mtc0 $2,$23;":"=r"(config)::"$2"); 

        /*
         * Do the next twice for two reasons. First make sure we run from
         * cache. Second make sure synched on second update. (Pun intended!)
         */
        for(i = 2;  i != 0; i--) {
                cnt = now();
                timeout = 10000000;
		sec=inl(RTC_REG);
                                                                               
                                                                               
                do {
                        timeout--;
			cur=inl(RTC_REG);
                } while(timeout != 0 && cur == sec);
                                                                               
                cnt = now() - cnt;
                if(timeout == 0) {
                        break;          /* Get out if clock is not running */
                }
        }
                                                                               
	/*
	 *  Calculate the external bus clock frequency.
	 */
	if (timeout != 0) {
		md_pipefreq = cnt / 10000;
		md_pipefreq *= 20000;
		/* we have no simple way to read multiplier value
		 */
		printf("cpu freq=%d\n",md_pipefreq);
	}
	else
	  printf("timeout\n");

	asm volatile("mtc0 %0,$23;"::"r"(config)); 
	return 0;
}
