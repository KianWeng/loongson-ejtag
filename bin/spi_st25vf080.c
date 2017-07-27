#define SPI_BASE  0xbfe80000
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

#define SET_SPI(addr,val)        *(volatile char *)(SPI_BASE+addr)=val
#define GET_SPI(addr)            (*(volatile char *)(SPI_BASE+addr))
#define delay(...)

int write_sr(char val);
void spi_initw()
{ 
  	SET_SPI(SPSR, 0xc0); 
  	SET_SPI(PARAM, 0x40);             //espr:0100
 	SET_SPI(SPER, 0x04); //spre:01 
  	SET_SPI(PARAM2,0x01); 
  	SET_SPI(SPCR, 0x51);
}

void spi_initr()
{
  	SET_SPI(PARAM, 0x47);             //espr:0100
}




///////////////////read status reg /////////////////

int read_sr(void)
{
	int val;
	
	SET_SPI(SOFTCS,0x01);
	SET_SPI(TXFIFO,0x05);
	while((GET_SPI(SPSR))&RFEMPTY);

	val = GET_SPI(RXFIFO);
	SET_SPI(TXFIFO,0x00);

	while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);

	val = GET_SPI(RXFIFO);
	
	SET_SPI(SOFTCS,0x11);
      
	return val;
}


void wait_sr(void)
{
	int val;
	
	SET_SPI(SOFTCS,0x01);
	SET_SPI(TXFIFO,0x05);
	while((GET_SPI(SPSR))&RFEMPTY);

	val = GET_SPI(RXFIFO);
	do{
	SET_SPI(TXFIFO,0x00);

	while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);

	val = GET_SPI(RXFIFO);
	} while(val & 1);
	
	SET_SPI(SOFTCS,0x11);
      
	return val;
}


////////////set write enable//////////
int set_wren(void)
{
	int res;
	
	wait_sr();
	
	SET_SPI(SOFTCS,0x01);
	
	SET_SPI(TXFIFO,0x6);
       	while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY){
	}
	GET_SPI(RXFIFO);

	SET_SPI(SOFTCS,0x11);

	return 1;
}

///////////////////////write status reg///////////////////////
int write_sr(char val)
{
	int res;
	
	set_wren();
	
	wait_sr();
	
	SET_SPI(SOFTCS,0x01);

	SET_SPI(TXFIFO,0x01);
	SET_SPI(TXFIFO,val);

       	while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);
	GET_SPI(RXFIFO);
       	while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);
	GET_SPI(RXFIFO);

	SET_SPI(SOFTCS,0x11);
	
	return 1;
	
}

///////////erase all memory/////////////
int erase_all(void)
{
	int res;
	int i=1;
        spi_initw();
	set_wren();
	wait_sr();
	SET_SPI(SOFTCS,0x1);
	
	SET_SPI(TXFIFO,0xC7);
       	while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY){
      			
	}
	GET_SPI(RXFIFO);
	
	SET_SPI(SOFTCS,0x11);
        while(i++){
            if(read_sr() & 0x1 == 0x1){
                if(i % 10000 == 0)
                    printf(".");
            }else{
                printf("done...\n");
                break;
            }   
        }
	return 1;
}



void spi_read_id(void)
{
    unsigned char val;
    spi_initw();
    val = read_sr();
    while(val&0x01 == 1)
    {
        val = read_sr();
    }
    /*CE 0*/
    SET_SPI(SOFTCS,0x01);
    /*READ ID CMD*/
    SET_SPI(TXFIFO,0x9f);
    while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY){

    }
    GET_SPI(RXFIFO);

    /*Manufacturer’s ID*/
    SET_SPI(TXFIFO,0x00);
    while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY){

    }
    val = GET_SPI(RXFIFO);
    printf("Manufacturer's ID:         %x\n",val);

    /*Device ID:Memory Type*/
    SET_SPI(TXFIFO,0x00);
    while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY){

    }
    val = GET_SPI(RXFIFO);
    printf("Device ID-memory_type:     %x\n",val);
    
    /*Device ID:Memory Capacity*/
    SET_SPI(TXFIFO,0x00);
    while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY){

    }
    val = GET_SPI(RXFIFO);
    printf("Device ID-memory_capacity: %x\n",val);
    
    /*CE 1*/
    SET_SPI(SOFTCS,0x11);

}

void spi_write_byte(unsigned int addr,unsigned char data)
{
	int i;
    /*byte_program,CE 0, cmd 0x2,addr2,addr1,addr0,data in,CE 1*/
	unsigned char addr2,addr1,addr0;
        unsigned char val;
	addr2 = (addr & 0xff0000)>>16;
    	addr1 = (addr & 0x00ff00)>>8;
	addr0 = (addr & 0x0000ff);
        set_wren();
	//wait_sr();

	SET_SPI(SOFTCS,0x01);/*CE 0*/
	/*write 5 data to fifo, depth>4?*/

        SET_SPI(TXFIFO,0x2);/*byte_program */
        
        /*send addr2*/
        SET_SPI(TXFIFO,addr2);     
        /*send addr1*/
        SET_SPI(TXFIFO,addr1);
        /*send addr0*/
        SET_SPI(TXFIFO,addr0);

        /*send data(one byte)*/
       	SET_SPI(TXFIFO,data);


	for(i=0;i<5;i++)
	{
        while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);
        val = GET_SPI(RXFIFO);
	}
        
        /*CE 1*/
	SET_SPI(SOFTCS,0x11);

}


int spi_erase_area(unsigned int saddr,unsigned int eaddr,unsigned sectorsize)
{
	unsigned int addr;
       	spi_initw(); 

	for(addr=saddr;addr<eaddr;addr+=sectorsize)
	{

	SET_SPI(SOFTCS,0x11);

	set_wren();

	write_sr(0x00);

	wait_sr();

	set_wren();

	SET_SPI(SOFTCS,0x01);

	SET_SPI(TXFIFO,0xd8);
       	while((GET_SPI(SPSR))&RFEMPTY);
	GET_SPI(RXFIFO);
	SET_SPI(TXFIFO,addr >> 16);

       	while((GET_SPI(SPSR))&RFEMPTY);
	GET_SPI(RXFIFO);

	SET_SPI(TXFIFO,addr >> 8);
       	while((GET_SPI(SPSR))&RFEMPTY);
	GET_SPI(RXFIFO);

	SET_SPI(TXFIFO,addr);
	
       	while((GET_SPI(SPSR))&RFEMPTY);
	GET_SPI(RXFIFO);
	
	SET_SPI(SOFTCS,0x11);

	wait_sr();
	}
	SET_SPI(SOFTCS,0x11);
	delay(10);

	return 0;
}

int spi_write_area(int flashaddr,char *buffer,int size)
{
	int j;
	spi_initw();
	SET_SPI(0x5,0x10);
	write_sr(0x00);
        for(j=0;size > 0;flashaddr++,size--,j++)
        {
		if((j&0xffff)==0)printf("%x\n",j);
            spi_write_byte(flashaddr,buffer[j]);
        }

	SET_SPI(SOFTCS,0x11);
	delay(10);
	return 0;
}


int spi_read_area(int flashaddr,char *buffer,int size)
{
	int i;
	spi_initw();

	SET_SPI(SOFTCS,0x01);

	SET_SPI(TXFIFO,0x03);

        while((GET_SPI(SPSR))&RFEMPTY);
        GET_SPI(RXFIFO);
        
        SET_SPI(TXFIFO,flashaddr>>16);     
        while((GET_SPI(SPSR))&RFEMPTY);
        GET_SPI(RXFIFO);

        SET_SPI(TXFIFO,flashaddr>>8);     
        while((GET_SPI(SPSR))&RFEMPTY);
        GET_SPI(RXFIFO);

        SET_SPI(TXFIFO,flashaddr);     
        while((GET_SPI(SPSR))&RFEMPTY);
        GET_SPI(RXFIFO);
        

        for(i=0;i<size;i++)
        {
        SET_SPI(TXFIFO,0);     
        while((GET_SPI(SPSR))&RFEMPTY);
        buffer[i] = GET_SPI(RXFIFO);
        }

        SET_SPI(SOFTCS,0x11);
	delay(10);
	return 0;
}


int mymain(int flashaddr,int memaddr,int size)
{
spi_read_id();
erase_all();
printf("flashaddr=%x,mem=%x,size=%x\n",flashaddr,memaddr,size);
spi_write_area(flashaddr,memaddr,size);
return 0;
}



