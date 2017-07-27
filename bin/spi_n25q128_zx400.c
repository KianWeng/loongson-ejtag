long SPI_BASE = 0xffffffffbfe80000;
//#define SPI_BASE 0xffffffffbfe00220

#define SPCR      0x0
#define SPSR      0x1
#define TXFIFO    0x2
#define RXFIFO    0x2
#define SPER      0x3
#define PARAM     0x11
#define SOFTCS    0x10
#define PARAM2    0x12

#define RFEMPTY 1

#define SET_SPI(addr,val)        *(volatile char *)(SPI_BASE+addr)=val
#define GET_SPI(addr)            (*(volatile char *)(SPI_BASE+addr))

#define MYDBG printf("%s:%d\n", __FUNCTION__, __LINE__);

#define  CS_L    SET_SPI(SOFTCS,0x00)
#define  CS_H    SET_SPI(SOFTCS,0x01)
#define  SEND_CMD(x)     do{ SET_SPI(TXFIFO,(x));                    \
                            while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);  \
                                GET_SPI(RXFIFO);                        \
                        }while(0)

#define  GET_DATA(x)     do{ SET_SPI(TXFIFO,0xff);                    \
                            while((GET_SPI(SPSR)&RFEMPTY) == RFEMPTY);  \
                        (x) = GET_SPI(RXFIFO);                        \
                        }while(0)

#define  SEND_DATA(x) SEND_CMD(x)

int write_sr(char val);
void spi_write_byte(unsigned int addr,unsigned char data);
void spi_initw()
{ 
  	SET_SPI(SPSR, 0xc0); 
  	SET_SPI(PARAM, 0x40);             //espr:0100
 	SET_SPI(SPER, 0x05); //spre:01 
  	//SET_SPI(PARAM2,0x01); 
  	SET_SPI(SPCR, 0x51);
}



///////////////////read status reg /////////////////

int read_sr(void)
{
    unsigned char val;
    CS_L;
    SEND_CMD(0x05);
    GET_DATA(val);
    CS_H;
    return val;
}


int wait_sr(void)
{
    unsigned char val;

    CS_L;
    SEND_CMD(0x05);
    do{
        GET_DATA(val);
    }while(val&0x1);
    CS_H;
    return val;
}

////////////set write enable//////////
int set_wren(void)
{
    unsigned char val;
    wait_sr();
    CS_L;
    SEND_CMD(0x6);
    CS_H;

    CS_L;
    SEND_CMD(0x05);
    do{
        GET_DATA(val);
    }while(!(val&0x2));
    CS_H;
    return 1;
}

///////////////////////write status reg///////////////////////
int write_sr(char sr)
{
    unsigned char res,val;
    set_wren();
    wait_sr();
    CS_L;
    SEND_CMD(0x1);
    SEND_DATA(sr);
    CS_H;
    return 1;
}

///////////erase all memory/////////////
int erase_all(void)
{
    int res;
    int i=1,val;

    spi_initw();
    write_sr(0);
    set_wren();
    wait_sr();

    CS_L;
    SEND_CMD(0xc7);
    CS_H;

    while(i++){
        //printf("val---==%x\n",read_sr());
        if(read_sr() & 0x3){
            if(i % 10000 == 0)
                printf(".");
            res=100;while(res--);
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
    wait_sr();
    CS_L;
    SEND_CMD(0x9F);
    GET_DATA(val); 
    printf("Manufacturer's ID:         %x\n",val);
    GET_DATA(val); 
    printf("Device ID-memory_type:     %x\n",val);
    GET_DATA(val); 
    printf("Device ID-memory_capacity: %x\n",val);
    CS_H;
}

void spi_write_byte(unsigned int addr,unsigned char data)
{
    /*byte_prog$31m,CE 0, cmd 0x2,addr2,addr1,addr0,data in,CE 1*/
    unsigned char addr2,addr1,addr0;
    unsigned char val;
    addr2 = (addr & 0xff0000)>>16;
    addr1 = (addr & 0x00ff00)>>8;
    addr0 = (addr & 0x0000ff);
    set_wren();
    wait_sr();
    CS_L;

    SEND_CMD(0x2);
    SEND_DATA(addr2);
    SEND_DATA(addr1);
    SEND_DATA(addr0);
    SEND_DATA(data);
    CS_H;

}


int spi_erase_area(unsigned int saddr,unsigned int eaddr,unsigned sectorsize)
{
    unsigned int addr;
    spi_initw(); 

    for(addr=saddr;addr<eaddr;addr+=sectorsize)
    {
        printf(". ");
        set_wren();
        wait_sr();

        CS_L;
        SEND_CMD(0xd8);
        SEND_DATA(addr>>16);
        SEND_DATA(addr>>8);
        SEND_DATA(addr);
        CS_H;
    }
    printf("\n");
    return 0;
}

int spi_write_area(int flashaddr,char *buffer,int size)
{
    int j,i;
    spi_initw();

    write_sr(0x00);

    for(j=0;size > 0;flashaddr++,size--,j++)
    {
        if((j&0xffff)==0) printf("%x\n",j);
        spi_write_byte(flashaddr,buffer[j]);
    }

    return 0;
}


int spi_read_area(int flashaddr,char *buffer,int size)
{
    int i;
    spi_initw();

    CS_L;
    SEND_CMD(0x03);
    SEND_DATA(flashaddr>>16);
    SEND_DATA(flashaddr>>8);
    SEND_DATA(flashaddr);
    for(i=0;i<size;i++)
    {
        GET_DATA(buffer[i]);
    }
    CS_H;

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

    printf("flashaddr=0x%x,mem=0x%lx,size=0x%x spibase=0x%lx\n",flashaddr,memaddr,size,SPI_BASE);
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

