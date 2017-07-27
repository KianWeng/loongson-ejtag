long SPI_BASE = 0xffffffffbfe80000;

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

int dll = 5;
int write_sr(char val);
void spi_initw()
{ 
  	SET_SPI(SPSR, 0xc0); 
  	SET_SPI(PARAM, 0x40);             //espr:0100
 	SET_SPI(SPER, (0x05&~0x3)|((dll>>2)&3)); //spre:01 
  	SET_SPI(PARAM2,0x01); 
  	SET_SPI(SPCR, (0x51&~3)|(dll&3));
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

int spi_write_bytes(unsigned int addr,unsigned char *buffer, int size)
{
	int i;
    /*byte_program,CE 0, cmd 0x2,addr2,addr1,addr0,data in,CE 1*/
	unsigned char addr2,addr1,addr0;
        unsigned char val;
	addr2 = (addr & 0xff0000)>>16;
    	addr1 = (addr & 0x00ff00)>>8;
	addr0 = (addr & 0x0000ff);
	size = (size>(256-addr0))?(256-addr0):size;
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

	for(i=0;i<4;i++)
	{
        while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);
        val = GET_SPI(RXFIFO);
	}

	for(i=0;i<size;i++)
	{
        /*send data(one byte)*/
       	SET_SPI(TXFIFO,buffer[i]);


        while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);
        val = GET_SPI(RXFIFO);
	}
        
        /*CE 1*/
	SET_SPI(SOFTCS,0x11);
        wait_sr();
	return size;	
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
	int cnt;
	spi_initw();
	//SET_SPI(SOFTCS,0xff);
	write_sr(0x00);
        for(;size > 0;)
        {
		if((flashaddr&0xffff)==0)printf("%x\n",flashaddr);
            cnt = spi_write_bytes(flashaddr,buffer, size);
	    size -= cnt;
	    flashaddr += cnt;
	    buffer += cnt;
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


int mymain(int flashaddr,unsigned long memaddr,int size, unsigned long spibase, char *incmd)
{
	int argc = *(int *)ARGC_REG;
	char *cmd = "iEp";

    if(argc < 3)
    { 
        printf("missing args: flashaddr memaddr size [spibase]\n");
	return -1;
    }
   
    if(argc >= 4)
	SPI_BASE = spibase;
    if(argc >= 5)
       cmd = incmd;

    printf("flashaddr=0x%x,mem=0x%lx,size=0x%x spibase=0x%lx cmd=%s\n",flashaddr,memaddr,size,SPI_BASE,cmd);
    for(;*cmd;cmd++)
    {
	    switch(*cmd)
	    {
		    case 'i':
			    printf("\n\nget flash IDS\n");
			    spi_read_id();
			    break;
		    case 'E':
			    printf("\n\nerase flash");
			    erase_all();
			    break;
		    case 'e':
			    printf("\n\nerase flash area");
			    spi_erase_area(0,size,0x10000);
			    break;
		    case 'p':
			    printf("\n\nwrite BIOS to flash\n");
			    spi_write_area(flashaddr,memaddr,size);
			    break;
		    case 'r':
			    spi_read_area(flashaddr,memaddr,size);
			    break;
	    }

    }
    printf("\n");
    return 0;
}
