/*
 *arch/mips/loongson/sb2f-board/gpio.c
 *LOONSON LS1B GPIO driver 
 *
 *
 */

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

#define PIN_IDX(pin)        ((pin)/32)
#define PIN_OFFSET(pin)     (0x1<<((pin)%32))

#define CCLK 1
#define INIT 7
#define PRG 0
#define BUSY 3
#define DONE 2
#define DATA0 12
#define DATA1 13
#define DATA2 14
#define DATA3 15
#define DATA4 11
#define DATA5 10
#define DATA6 9
#define DATA7 8
#define M0 4 //always 2
#define M1 5 //always 2
#define M2 6 //always 2

#define SET_H(x)   ls1b_gpio_write_pin(x,GPIO_HIGH)
#define SET_L(x)   ls1b_gpio_write_pin(x,GPIO_LOW)
#define GET_VAL(x) ls1b_gpio_read_input_pin(x)
#define CLK_H      ls1b_gpio_write_pin(CCLK,GPIO_HIGH)
#define CLK_L      ls1b_gpio_write_pin(CCLK,GPIO_LOW)

#define bit(n) (((data>>n)&1)<<DATA ## n)
#define msk(n) (1<<DATA ## n)
#define wdata (bit(0)|bit(1)|bit(2)|bit(3)|bit(4)|bit(5)|bit(6)|bit(7))
#define mask (msk(0)|msk(1)|msk(2)|msk(3)|msk(4)|msk(5)|msk(6)|msk(7))

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


void send_byte_clk_l(unsigned char data)
{
#if 1 //data modify
    data = (data << 4) | (data>>4);
    data = ((data<<2)&0xcc) | ((data>>2)&0x33);
    data  = ((data<<1)&0xaa) | ((data>>1)&0x55);
#endif
    _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & ~(mask|(1<<CCLK)))|wdata); 
}


void send_byte(unsigned int data)
{
#if 1 //data modify
    data = (data << 4) | (data>>4);
    data = ((data<<2)&0xcc) | ((data>>2)&0x33);
    data  = ((data<<1)&0xaa) | ((data>>1)&0x55);
#endif

    _reg_writel(GPIO_OUT_VAL,(_reg_readl(GPIO_OUT_VAL) & ~mask)|wdata); 
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
int prg_some(unsigned char *buf,int len)
{
    int num,i;
    for(num=0;num<len;num++){
//        CLK_L;
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
            for(i=0;i<64;i++){
                CLK_L;
                CLK_H;
            }
            printf("FPGA CFG done...\n");
            break;
        }

        if((num % 0x20000 == 0) && num)
            printf("0x%08x\n",num);
    }
    printf("0x%08x\n",num);

}

void gpio_test(unsigned int data)
{
    printf("test==0x%08x\n",data);
    ls1b_gpio_init_register_pin(CCLK,GPIO_OUTPUT, (data>>CCLK) & 0x1);
    ls1b_gpio_init_register_pin(INIT,GPIO_OUTPUT, (data>>INIT) & 0x1);
    ls1b_gpio_init_register_pin(PRG,GPIO_OUTPUT,  (data>>PRG) & 0x1);
    ls1b_gpio_init_register_pin(BUSY,GPIO_OUTPUT, (data>>BUSY) & 0x1);
    ls1b_gpio_init_register_pin(DONE,GPIO_OUTPUT, (data>>DONE) & 0x1);
    ls1b_gpio_init_register_pin(DATA0,GPIO_OUTPUT,(data>>DATA0) & 0x1);
    ls1b_gpio_init_register_pin(DATA1,GPIO_OUTPUT,(data>>DATA1) & 0x1);
    ls1b_gpio_init_register_pin(DATA2,GPIO_OUTPUT,(data>>DATA2) & 0x1);
    ls1b_gpio_init_register_pin(DATA3,GPIO_OUTPUT,(data>>DATA3) & 0x1);
    ls1b_gpio_init_register_pin(DATA4,GPIO_OUTPUT,(data>>DATA4) & 0x1);
    ls1b_gpio_init_register_pin(DATA5,GPIO_OUTPUT,(data>>DATA5) & 0x1);
    ls1b_gpio_init_register_pin(DATA6,GPIO_OUTPUT,(data>>DATA6) & 0x1);
    ls1b_gpio_init_register_pin(DATA7,GPIO_OUTPUT,(data>>DATA7) & 0x1);
    ls1b_gpio_init_register_pin(M0,GPIO_OUTPUT,   (data>>M0) & 0x1);
    ls1b_gpio_init_register_pin(M1,GPIO_OUTPUT,   (data>>M1) & 0x1);
    ls1b_gpio_init_register_pin(M2,GPIO_OUTPUT,   (data>>M2) & 0x1);
}

int mymain(char *cmd,unsigned char *buf,int len)
{
    int i;
    fpga_gpio_init();

    printf("cmd=%s\n", cmd);

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

    return 0;
}


