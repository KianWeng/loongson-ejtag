//tinyos 

#include "math.h"

#define CCLK 11
#define INIT 10
#define PRG 8
#define BUSY 12
#define DONE 9
#define DATA0 0
#define DATA1 1
#define DATA2 2
#define DATA3 3
#define DATA4 4
#define DATA5 5
#define DATA6 6
#define DATA7 7
#define M0 15 //always 2
#define M1 14 //always 2
#define M2 13 //always 2

#define PRG_DATA_MSK    ((0x1<<DATA0)|(0x1<<DATA1)|(0x1<<DATA2)|(0x1<<DATA3)|(0x1<<DATA4)|(0x1<<DATA5)|(0x1<<DATA6)|(0x1<<DATA7))


#define BPUT_SIG0    17 //out
#define BPUT_SIG1    18 //out
#define BPUT_SIG2    19 //out
#define BTAG         20 //in
#define BGET_SIG0    21 //in
#define BGET_SIG1    22 //in
#define BGET_SIG2    23 //in
#define BDATA0   24 //in
#define BDATA1   25 //in
#define BDATA2   26 //in
#define BDATA3   27 //in
#define BDATA4   28 //in
#define BDATA5   29 //in
#define BDATA6   30 //in
#define BDATA7   31 //in

#define REV_DATA_MSK    ((0x1<<BDATA0)|(0x1<<BDATA1)|(0x1<<BDATA2)|(0x1<<BDATA3)|(0x1<<BDATA4)|(0x1<<BDATA5)|(0x1<<BDATA6)|(0x1<<BDATA7))
#define SEND_SIG_MSK  ((0x1<<BPUT_SIG0)|(0x1<<BPUT_SIG1)|(0x1<<BPUT_SIG2))
#define GET_SIG_TAG_MSK  ((0x1<<BGET_SIG0)|(0x1<<BGET_SIG1)|(0x1<<BGET_SIG2)|(0x1<<BTAG))


#define F_RDY   0x3
#define F_EOF   0x1
#define F_PUT   0x4
#define F_ACK0   0x5      
#define F_ACK1   0x2      
#define F_DONE   0x6      
//#define F_CONT  0x7


#define FS_TAG0         0
#define FS_TAG1         1

#define FS_TAG_START    FS_TAG0
#define F_SR_START_B    F_RDY

#define ONCE_LEN 0x4000

//#define F_GET   0x2
//#define F_ERR   0x4
//#define F_BYTE  0x5

#define MSK_TAG 0x1     //0001
#define MSK_SR  0xe0    //1110

#define SR_2_SIG(x)     ((x)>>1)
#define SR_2_TAG(x)     ((x)&1)



#define PIN_OFFSET(pin)     (0x1<<((pin)%32))

#define GPIO_EN   0xbfd010c0
#define GPIO_DIR    0xbfd010d0
#define GPIO_IN_VAL 0xbfd010e0
#define GPIO_OUT_VAL 0xbfd010f0

#define GPIO_OUTPUT     1
#define GPIO_INPUT      0
#define GPIO_HIGH       1
#define GPIO_LOW        0

#define _reg_readl(x)         (*(volatile unsigned int *)(x))
#define _reg_writel(x,y)      ((*(volatile unsigned int *)(x))=(y))



#define SET_H(x)   ls1b_gpio_write_pin(x,GPIO_HIGH)
#define SET_L(x)   ls1b_gpio_write_pin(x,GPIO_LOW)
#define GET_VAL(x) ls1b_gpio_read_input_pin(x)
#define CLK_H      ls1b_gpio_write_pin(CCLK,GPIO_HIGH)
#define CLK_L      ls1b_gpio_write_pin(CCLK,GPIO_LOW)

void send_sig(unsigned char sig);
//register and init direction and the init value
int ls1b_gpio_init_register_pin(unsigned int pin,int output,int value)
{
    unsigned long flags,idx,offset,tmp,val,ret=-1;
    offset=PIN_OFFSET(pin);
    val = value? GPIO_HIGH : GPIO_LOW;
    //set direction
    tmp = _reg_readl(GPIO_DIR);
    if(!output){
        _reg_writel(GPIO_DIR,tmp|offset);
    }else{
        _reg_writel(GPIO_DIR,tmp&(~offset));
    }
    //if output ; set out-value.   if input;NULL
    if(output){
        tmp = _reg_readl(GPIO_OUT_VAL);
        if(val)
            _reg_writel(GPIO_OUT_VAL,tmp|offset);
        else
            _reg_writel(GPIO_OUT_VAL,tmp&(~offset));
    }
    //enable the PIN-GPIO func  
    _reg_writel(GPIO_EN,_reg_readl(GPIO_EN)|offset);
    return 0;
}
/*ls1b_gpio_read_input_pin  only read the INPUT VALUE register*/
int ls1b_gpio_read_input_pin(unsigned int pin)
{
    unsigned long offset;
    offset=PIN_OFFSET(pin);
    return ((_reg_readl(GPIO_IN_VAL) & offset)?GPIO_HIGH:GPIO_LOW);
}

/*ls1b_gpio_write_pin  only write the OUTPUT VALUE register*/
int ls1b_gpio_write_pin(unsigned int pin, int value)
{
   unsigned long offset;
    offset=PIN_OFFSET(pin);
    if(value)
        _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL))|offset);
    else
        _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL))&(~offset));
    _reg_readl(GPIO_OUT_VAL);
    return 0;
}


void send_byte_clk_l(unsigned short data)
{
#if 1 //data modify
    data = (data << 4) | (data>>4);
    data = ((data<<2)&0xcc) | ((data>>2)&0x33);
    data  = ((data<<1)&0xaa) | ((data>>1)&0x55);
#endif
    _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & 0xfffff700)|(data&0xff)); 
}


void send_byte(unsigned char data)
{
#if 1 //data modify
    data = (data << 4) | (data>>4);
    data = ((data<<2)&0xcc) | ((data>>2)&0x33);
    data  = ((data<<1)&0xaa) | ((data>>1)&0x55);
#endif
    //_reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & 0xffffff00)|data); 
    _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & (~(PRG_DATA_MSK)))|(data&0xff)); 
}
void send_m2(unsigned char data)
{
    ls1b_gpio_write_pin(M0,(data>> 0)&0x1);
    ls1b_gpio_write_pin(M1,(data>> 1)&0x1);
    ls1b_gpio_write_pin(M2,(data>> 2)&0x1);
}

void fpga_gpio_init(void)
{
    ls1b_gpio_init_register_pin(CCLK,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(PRG,GPIO_OUTPUT,GPIO_HIGH);
    ls1b_gpio_init_register_pin(INIT,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BUSY,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DONE,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA0,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA1,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA2,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA3,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA4,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA5,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA6,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(DATA7,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(M2,GPIO_OUTPUT,GPIO_HIGH);
    ls1b_gpio_init_register_pin(M1,GPIO_OUTPUT,GPIO_HIGH);
    ls1b_gpio_init_register_pin(M0,GPIO_OUTPUT,GPIO_LOW);
}

void prg_init(void)
{
    int i=0;

    CLK_L;
    send_m2(6);
    CLK_H;
   
    CLK_L;
    SET_L(PRG);
    CLK_H;

    i=64;
    do{ 
        CLK_L;
        CLK_H;
    }while(i--);
    
    CLK_L;
    SET_H(PRG);
    CLK_H;


    while(GET_VAL(INIT)==GPIO_LOW);
    printf("FPGA clear init ok...\n");
}
int prg_some(unsigned short *buf,int len)
{
    int num,i,ret=F_PUT;
    for(num=0;num<len;num++){
        send_byte_clk_l(buf[num]); 
        CLK_H;
#if 0
        while(GET_VAL(BUSY) == GPIO_HIGH){
            printf("busy..0x%x...\n",num);
            //CLK_L;
            //CLK_H;
            delay(2);
        }
#endif        
        if(GET_VAL(DONE) == GPIO_HIGH){
            send_sig(F_DONE);
            for(i=0;i<64;i++){
                CLK_L;
                CLK_H;
            }
            ret = F_DONE;
            break;
        }//if
    }//for
    return ret; 
}




void fpga_soc_gpio_init_b(void)
{
    ls1b_gpio_init_register_pin(BPUT_SIG0,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BPUT_SIG1,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BPUT_SIG2,GPIO_OUTPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BTAG,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BGET_SIG0,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BGET_SIG1,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BGET_SIG2,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA0,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA1,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA2,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA3,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA4,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA5,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA6,GPIO_INPUT,GPIO_LOW);
    ls1b_gpio_init_register_pin(BDATA7,GPIO_INPUT,GPIO_LOW);

}
void fpga_soc_sr_init_b(void)
{
    send_sig(F_SR_START_B);
}
void send_sig(unsigned char sig)
{
//  _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & 0xfff1ffff)|(sig<<BPUT_SIG0)); 
  _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & (~(SEND_SIG_MSK)))|(sig<<BPUT_SIG0)); 
}
unsigned char get_sig(void)
{
    return((_reg_readl(GPIO_IN_VAL) >> BTAG) & 0xf); 
}

unsigned char get_byte_realy(void)
{
    return (_reg_readl(GPIO_IN_VAL) >>BDATA0);
}
int get_byte(unsigned char tag,unsigned short*data)
{
    unsigned char sr=0;
    int ret;
    while(tag != ( sr=get_sig() , sr & MSK_TAG));
    switch(SR_2_SIG(sr)){
        case F_PUT:
            *data = get_byte_realy();
            send_sig(tag?F_ACK1:F_ACK0);
            ret = F_PUT;
            break; 
#if 0
        case F_ERR:
            ret = F_ERR;
            break;
#endif
        case F_EOF:
        default:
            //printf("\n----sr==%x\n\n",SR_2_SIG(sr));
            ret = F_EOF;
            break;
    }
    return ret;
}
void test_data(unsigned short *buf,int len)
{
    static int num=0;
    int j;
    unsigned short data;

    for(j=0;j<len;j++){
    data = buf[j];
    data = (data << 4) | (data>>4);
    data = ((data<<2)&0xcc) | ((data>>2)&0x33);
    data  = ((data<<1)&0xaa) | ((data>>1)&0x55);
        *(buf+0x1000+num)=data&0xff;
        num++;
    }
    printf("num==%x\n",num);
}
int rev_data(unsigned short *buf,unsigned int len)
{
    unsigned int i=0,prgret=0,ret=0;
    unsigned int sr=0,rev_len=0,times=0;
    //printf("%s---%d---F_RDY,len=0x%08x\n",__func__,__LINE__,len);
    while(F_RDY!=SR_2_SIG(get_sig()));
    while(F_PUT!=SR_2_SIG(get_sig()));
   // printf("%s---%d---F_RDY.ok...\n",__func__,__LINE__);

    sr=FS_TAG_START;
    ret = F_EOF;
    for(i=0;i<len;i++){
        if(i>=(ONCE_LEN)){
            //test_data(buf,i);
            prg_some(buf,i);
            i=0;
            /*
            times++; 
            printf(".");
            if(times%16 == 0){
                printf(": 0x%08x\n",rev_len);
            }
            */
        }
       // rev_len++;
        ret = get_byte(sr,&buf[i]);
        sr=!sr;
        if(ret!=F_PUT){
            //test_data(buf,i);
            prg_some(buf,i);
            break;
        }
    }
    return i;
}

void test_iobus(unsigned char *buf,int len)
{
   fpga_soc_gpio_init_b();//for rev-data
  fpga_soc_sr_init_b(); //for-rev-data
    fpga_gpio_init();  //for-cfg-fpga
    prg_init();

    rev_data(buf,-1);
}


int mymain(unsigned int buf1,int len)
{
    int i;
    unsigned char *buf=(unsigned char *)buf1;
//    fpga_gpio_init();

    printf("fpga-iobus test...0x%08x:0x%08x\n",buf,len);
    test_iobus(buf,len);
#if 0
    switch(*cmd)
    {
        case 'i':
            prg_init();
            break;
        case 'p':
            prg_some(buf,len);
            break;
        case 't':
            gpio_test(len);
            break;
        default:
            break;
    }
#endif
    return 0;
}



