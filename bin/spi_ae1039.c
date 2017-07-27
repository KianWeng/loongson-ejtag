#define SPI_BASE 0xbfe80000
#define PMON_ADDR 0xa1000000
#define FLASH_ADDR 0x000000

#define SPCR      0x0
#define SPSR      0x1
#define TXFIFO    0x2
#define RXFIFO    0x2
#define SPER      0x3
#define PARAM     0x4
#define SOFTCS    0x5
#define PARAM2    0x6

#define RFEMPTY 1

#define MYDBG printf("%d\n",__LINE__);

typedef unsigned char u8;
//#define SET_SPI(addr,val)        *(volatile u8 *)(SPI_BASE+addr) = ((u8)val & 0xff)
#define GET_SPI(addr)            *(volatile u8 *)(SPI_BASE+addr)
void SET_SPI(int addr, u8 val)
{
	if((addr&0xf)==5) {
		if(val&0xf0)
		  val=0x11;
		else
		  val=0x1;
	}
	*(volatile u8 *)(SPI_BASE+addr) = ((u8)val & 0xff);
}

///////////////////read status reg /////////////////
static void init_spi_flash() 
{
	static int inited = 0;
	if(inited)
		return;
	
	//printf("==SPI INIT==\n");
	SET_SPI(SPSR, 0xc0); 
  	//printf("SPSR:%x\n",GET_SPI(SPSR));
	
  	//SET_SPI(PARAM, 0x40);             //espr:0100
  	SET_SPI(PARAM, 0x20);             //espr:0100
  	//printf("PARAM:%x\n",GET_SPI(0x4));
	
 	SET_SPI(SPER, 0x05); //spre:01 
	
  	SET_SPI(PARAM2,0x01); 
  	//printf("====CS:%x\n",GET_SPI(0x5));
	
  	SET_SPI(SPCR, 0x50);
  	//printf("SPCR:%x\n",GET_SPI(0x0));

     //   write_sr(0);

	//SET_SPI(SPCR, 0x5e);
	//SET_SPI(SPER, 0x00);

	inited = 1;
	return;
}

int read_sr(void)
{
	int val=0;
	init_spi_flash();

//============================================
	SET_SPI(SOFTCS,0x01);

	SET_SPI(TXFIFO,0x05);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
	}
	GET_SPI(RXFIFO);
	
	SET_SPI(TXFIFO,0x00);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
	}
	val = GET_SPI(RXFIFO);
	
	SET_SPI(SOFTCS,0x11);
      
//============================================
#if 1
	SET_SPI(SOFTCS,0x01);

	SET_SPI(TXFIFO,0x35);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
	}
	GET_SPI(RXFIFO);
	
	SET_SPI(TXFIFO,0x00);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
	}
	val |= GET_SPI(RXFIFO)<<8;
	
	SET_SPI(SOFTCS,0x11);
      
	//printf("SR:%x\n", val);
#endif

	return val;
}

////////////set write enable//////////
int set_wren(void)
{
	int res;
	
	res = read_sr();
	while(res&0x01 == 1)
	{
		res = read_sr();
	}
	
	SET_SPI(SOFTCS,0x01);
	
	SET_SPI(TXFIFO,0x6);
       	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
	}
	GET_SPI(RXFIFO);

	SET_SPI(SOFTCS,0x11);

	return 1;
}

///////////////////////write status reg///////////////////////

///////////erase all memory/////////////
int erase_all(void)
{
	int res,i=0;
	
	set_wren();
	res = read_sr();
	while(res&0x01 == 1)
	{
		res = read_sr();
	}
	
	SET_SPI(SOFTCS,0x01);
	
	SET_SPI(TXFIFO,0xC7);
    while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	
	SET_SPI(SOFTCS,0x11);

	printf("Erasing chip");
	res = read_sr();
	while(res&0x01 == 1)
	{
		res = read_sr();
		if((i%5000) == 0)
			printf(".");
		i++;
		if((i%600000) == 0)
			printf("\n");
	}
	printf("Done!\n");

	return 1;
}

int erase_sector(char addr2,char addr1,char addr0)
{
	int res;
	
	set_wren();
	res = read_sr();
	while(res&0x01 == 1)
	{
		res = read_sr();
	}
	
	SET_SPI(SOFTCS,0x01);
	
	SET_SPI(TXFIFO,0x20);
       	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	SET_SPI(TXFIFO,addr2);
       	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	SET_SPI(TXFIFO,addr1);
       	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	SET_SPI(TXFIFO,addr0);
	
       	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	
	SET_SPI(SOFTCS,0x11);

	return 1;

}

int write_pmon(void)
{
	int ret=0;
	long int i=0,j=0;
	int a=0;
	int val;
	unsigned char *data;
	unsigned char addr2,addr1,addr0;
	int addr=0;
	
//	spi_initw();
	init_spi_flash();
	val = read_sr();
	while(val&0x01 == 1)
	{
		val = read_sr();
	}

	SET_SPI(SOFTCS,0x01);
	
// read flash id command
	SET_SPI(TXFIFO,0xab);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
				
	}
	GET_SPI(RXFIFO);
	
        SET_SPI(TXFIFO,0x00);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
				
	}
	GET_SPI(RXFIFO);
	
        SET_SPI(TXFIFO,0x00);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	
        SET_SPI(TXFIFO,0x00);
	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
				
	}
        GET_SPI(RXFIFO);
// commad end
		
        SET_SPI(TXFIFO,0x00);
        while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
        }
        val = GET_SPI(RXFIFO);
        printf("Flash ID:%x\n",val);
//     while ((*(volatile unsigned char *)(0xbfe80001))&0x01);
	val = GET_SPI(SPSR);
	//printf("====spsr value:%x\n",val);
	
	SET_SPI(SOFTCS,0x11);
// erase the flash     
	//write_sr(0x00);
	erase_all();
	
/*	addr2 = addr1 = addr0 = 0x00;
	
	for(i=0;i<64;i++){
		erase_sector(addr2,addr1,addr0);
		addr2++;
	}
*/

    printf("Programming");
// write flash
	i=j=0;
	while(1)
	{
		a=256*i;
		addr2 = ((FLASH_ADDR+a)&0xff0000)>>16;
		addr1 = ((FLASH_ADDR+a)&0x00ff00)>>8;
		addr0 = ((FLASH_ADDR+a)&0x0000ff);

		set_wren();
		val = read_sr();
		while(val&0x01 == 1)
		{
			val = read_sr();
		}
		
		SET_SPI(SOFTCS,0x01);
		
// writing sector command
		SET_SPI(TXFIFO,0x2);		
        	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
		}
		val = GET_SPI(RXFIFO);

// addr
		SET_SPI(TXFIFO,addr2);     
        	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
        	}
	        val = GET_SPI(RXFIFO);
		SET_SPI(TXFIFO,addr1);
        	while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	        }
	        val = GET_SPI(RXFIFO);
		SET_SPI(TXFIFO,addr0);
	        while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	        }
	        val = GET_SPI(RXFIFO);
// addr end
		
		if((i%64)==0) printf(".");
//		printf("===PP send \n");
		while(j<256)
		{
/*		     	if(addr%16 == 0 ){
				printf("    %08x\n",addr);
			}
*/
			if(addr == 0x00060000){
				printf("Done!\n");
				ret=1;
				break;
			}

			data =(unsigned char*) (PMON_ADDR+a+j);
//			printf(" %02x ",*data);
			
			SET_SPI(TXFIFO,*data);
	                while((GET_SPI(SPSR))&RFEMPTY == RFEMPTY){
      			
	                }
	                val = GET_SPI(RXFIFO);
			j++;
			addr++;	
			
		}
		if(ret) break;
		j=0;
		SET_SPI(SOFTCS,0x11);
		i++;
	
	}

	return 1;
}



int mymain()
{
	printf("*****SPI Flash Programming Proccess*****\n");
	write_pmon();
	return 0;
}




