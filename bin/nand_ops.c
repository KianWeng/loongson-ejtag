
#define MYDBG 
//printf("%s:%x\n",__func__,__LINE__)

#define _WL(x,y)    (*((volatile unsigned int*)(x)) = (y))
#define _RL(x,y)    ((y) = *((volatile unsigned int*)(x)))

#define NAND_WL(x,y)    _WL((x) + NAND_BASE,(y))
#define NAND_RL(x,y)    _RL((x) + NAND_BASE,(y))
#define DMA_WL(x,y)    _WL((x) + DMA_DESP,(y))
#define DMA_RL(x,y)    _RL((x) + DMA_DESP,(y))



#define RAM_ADDR            0xa4100000
#define DMA_DESP            0xa4001000
#define NAND_TMP            0xa4800000

#define BLOCK_TO_PAGE(x)  ((x)<<6)
#define PAGE_TO_BLOCK(x)  ((x)>>6)

#define PAGE_TO_FLASH(x)  ((x)<<11)
#define PAGE_TO_FLASH_H(x)  ((x)>>21)

#define PAGE_TO_FLASH_S(x)  ((x)<<12)
#define PAGE_TO_FLASH_H_S(x)  ((x)>>20)

#define FLASH_TO_PAGE(x)  ((x)>>11)
#define VT_TO_PHY(x)      ((x) & 0x0fffffff)   

#define PAGES_A_BLOCK       0x40

#define NAND_BASE           0xbfe78000
#define CMD                 0x0
#define ADDRL               0x4
#define ADDRH               0x8
#define TIMING              0xc
#define IDL                 0x10
#define IDH                 0x14
#define PARAM               0x18
#define OPNUM               0x1c
#define CS_RDY_MAP          0x20
#define DMA_ACCESS_ADDR     0x40

#define DMA_ORDER           0x0
#define DMA_SADDR           0x4
#define DMA_DEV             0x8
#define DMA_LEN             0xc
#define DMA_SET_LEN         0x10
#define DMA_TIMES           0x14
#define DMA_CMD             0x18

#define STOP_DMA            (_WL(0xbfd01160,0x10))
#define START_DMA(x)       do{  _WL(0xbfd01160,(VT_TO_PHY(x)|0x8));}while(0)
#define ASK_DMA(x)          (_WL(0xbfd01160,(VT_TO_PHY(x)|0x4)))

#define en_count(tmp)            do{asm volatile("mfc0 %0,$23;li $2,0x2000000;or $2,%0;mtc0 $2,$23;":"=r"(tmp)::"$2"); }while(0)
#define dis_count(tmp)           do{asm volatile("mtc0 %0,$23;"::"r"(tmp)); }while(0)

//#define SET_GPIO0_OUT(x)    do{x=(x==0?0:1);x=(*((volatile unsigned int*)0xbfd010f0) & ~0x1)|x; _WL(0xbfd010f0,(x));}while(0)
//#define SET_GPIO1_IN(x)    ((*((volatile unsigned int*)0xbfd010e0) & 0x2)>>1)



typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int  u32;
typedef long long u64;

int is_bad_block(u32 block);
int erase_block(u32 block);
int read_pages(u32 ram,u32 page,u32 pages,u8 flag);//flag:0==main 1==spare 2==main+spare
int write_pages(u32 ram,u32 page,u32 pages,u8 flag);//flag:0==main 1==spare 2==main+spare
int read_nand(u32 ram,u32 flash, u32 len,u8 flag);
int write_nand(u32 ram,u32 flash,u32 len,u8 flag);
int erase_nand(u32 flashaddr,u32 len);

int chipsize=0x08000000;

void nand_udelay(unsigned int us)
{
    int tmp=0;
    en_count(tmp);
    udelay(us);
    dis_count(tmp);
}
static void nand_send_cmd(u32 cmd,u32 page)//Send addr and cmd
{
    if(cmd & (0x1<<9)){
        if(cmd &(0x1<<8)){
            NAND_WL(ADDRL,PAGE_TO_FLASH_S(page));
        }else{
            NAND_WL(ADDRL,PAGE_TO_FLASH_S(page)+0x800);
        }
//        NAND_WL(ADDRH,PAGE_TO_FLASH_H_S(page));
    }else{
        NAND_WL(ADDRL,PAGE_TO_FLASH(page));
//        NAND_WL(ADDRH,PAGE_TO_FLASH_H_S(page));
    }
    {
        int val=0;
//    printf("flashaddr==0x%08x,cmd==%d\n",NAND_RL(ADDRL,val),cmd);
    }

    NAND_WL(CMD,cmd & (~0xff));
    NAND_WL(CMD,cmd);
}
#if 0
void gpio_init(void)
{
    _WL(0xbfd010c0,0x3);//enable gpio0 and gpio1
    _WL(0xbfd010d0,0x2);//gpio0 output,gpio1 intput
    
}
#endif
static u8 rdy_status(void)
{
    return ((*((u32*) (NAND_BASE+CMD)) & (0x1<<16)) == 0 ? 0:1);
}
static u8 nand_op_ok(u32 len,u32 ram)
{
    u32 ask=0,dmalen=0,dmastatus=0;
  //  nand_udelay(1000);
    while(1){
    ASK_DMA(DMA_DESP);
    _RL(DMA_DESP+0x4,ask);
    _RL(DMA_DESP+0xc,dmalen);
    _RL(DMA_DESP+0x18,dmastatus);
//    printf("ask==0x%08x,damlen==0x%08x,dmastatus==0x%08x\n",ask,dmalen,dmastatus);
//    printf("ask should==0x%08x,len==0x%08x\n", (VT_TO_PHY(ram)+((len+3)/4)*4),len);
    if(ask == (VT_TO_PHY(ram)+((len+3)/4)*4)){break;}else{return 0;}
//    return 0;
    nand_udelay(60);
    }
    while(!rdy_status())
        nand_udelay(20);
    return 0;
}
static void dma_config(u32 len,u32 ddr,u32 cmd)
{
//    MYDBG;
    DMA_WL(DMA_ORDER,0);
//    MYDBG;
    DMA_WL(DMA_SADDR,VT_TO_PHY(ddr));
//    MYDBG;
    DMA_WL(DMA_DEV,0x1fe78040);
//    MYDBG;
    DMA_WL(DMA_LEN,(len+3)/4);
//    MYDBG;
    DMA_WL(DMA_SET_LEN,0);
//    MYDBG;
    DMA_WL(DMA_TIMES,1);
//    MYDBG;
    DMA_WL(DMA_CMD,cmd);
//    MYDBG;
}
int is_bad_block(u32 block)
{
    u32 page=0;
    page = BLOCK_TO_PAGE(block);
    read_pages(RAM_ADDR,page,1,1);
    if(*((u16*) RAM_ADDR)!= 0xffff){return 1;}
    return 0;
}
int erase_block(u32 block)
{
    u32 flash_l=0,flash_h=0;
    NAND_WL(TIMING,0x206);
    NAND_WL(OPNUM,0x1);
    //printf("block==0x%08x\n",block);
    nand_send_cmd(0x9,BLOCK_TO_PAGE(block));
    //MYDBG
    nand_udelay(30);
    while(!rdy_status())
        nand_udelay(30);
}
int read_pages(u32 ram,u32 page,u32 pages,u8 flag)//flag:0==main 1==spare 2==main+spare
{
    u32 len=0,dma_cmd=0,nand_cmd=0;
    u32 ret = 0,tmp;
    switch(flag){
        case 0:
            len = pages * 0x800;
            nand_cmd = 0x103;
            break;
        case 1:
            len = pages * 0x40;
            nand_cmd = 0x203;
            break;
        case 2:
            len = pages * 0x840;
            nand_cmd = 0x303;
            break;
        default:
            len = 0;
            nand_cmd = 0;
            break;
    }
//    MYDBG;
    dma_config(len,ram,dma_cmd);
//    MYDBG;
    STOP_DMA;
//    MYDBG;
    nand_udelay(5);
//    MYDBG;
    START_DMA(DMA_DESP);
    nand_udelay(10);
//    MYDBG;
//    _RL(0xbfd01160,tmp);
//    MYDBG;
//    printf("0xbfd001160===0x%08x\n",*(volatile unsigned int *)(0xbfd01160));
    NAND_WL(TIMING,0x206);
    NAND_WL(OPNUM,len);
    nand_send_cmd(nand_cmd,page);
    nand_udelay(20000);
    MYDBG;
    ret = nand_op_ok(len,ram);
    MYDBG;
   if(ret){
        printf("nandread 0x%08x page 0x%08x pages have some error\n",page,pages);
   } 
   return ret;
}
#if 0
int write_pages(u32 ram,u32 page,u32 pages,u8 flag)
{
    u32 len=0,dma_cmd=0x1000,nand_cmd=0;
    u32 ret=0;
    switch(flag){
        case 0:
            len = pages * 0x800;
            nand_cmd = 0x105;
            break;
        case 1:
            len = pages * 0x40;
            nand_cmd = 0x205;
            break;
        case 2:
            len = pages * 0x840;
            nand_cmd = 0x305;
            break;
        default:
            len = 0;
            nand_cmd = 0;
            break;
    }
    printf("len==0x%08x,pages==0x%08x\n",len,pages);
    dma_config(len,ram,dma_cmd);
    STOP_DMA;
    nand_udelay(10);
    START_DMA(DMA_DESP);
    NAND_WL(TIMING,0x412);
    NAND_WL(OPNUM,len);
    nand_send_cmd(nand_cmd,page);
    nand_udelay(1000);
        MYDBG;
    ret = nand_op_ok(len,ram);
        MYDBG;
   if(ret){
        printf("nandwrite 0x%08x page 0x%08x pages have some error\n",page,pages);
   } 
   return ret;
}
#endif

int write_pages(u32 ram,u32 page,u32 pages,u8 flag)
{
    u32 len=0,dma_cmd=0x1000,nand_cmd=0;
    u32 ret=0,step=1;
    int val=pages;
#if 1
    switch(flag){
        case 0:
            len = step*0x800;
            nand_cmd = 0x105;
            break;
        case 1:
            len =  step*0x40;
            nand_cmd = 0x205;
            break;
        case 2:
            len = step*0x840;
            nand_cmd = 0x305;
            break;
        default:
            len = 0;
            nand_cmd = 0;
            break;
    }
#endif
    for(;val>0;val-=step){
//        printf("len==0x%08x,pages==0x%08x\n",len,val);
        dma_config(len,ram,dma_cmd);
        STOP_DMA;
        nand_udelay(10);
        START_DMA(DMA_DESP);
        NAND_WL(TIMING,0x206);
        NAND_WL(OPNUM,len);
        nand_send_cmd(nand_cmd,page);
        nand_udelay(20000);
        ret = nand_op_ok(len,ram);
        ram+=len;
        page+=step;
        MYDBG;
        if(ret){
            printf("nandwrite 0x%08x page 0x%08x pages have some error\n",page,pages);
        }
    } 
    return ret;
}
int error_check(u32 ram,u32 flash,u32 len)
{ 
    if(FLASH_TO_PAGE(flash) >= 0xffffffff){
        printf("the FLASH addr is big,this program don't work on this FLASH addr...\n");
        return -1;
    } 
    if(flash & 0x7ff){
        printf("the FLASH addr don't align/* Need 0x800B alignment*/\n");
        return -1;
    }
    if(ram & 0x1f){
        printf("the RAM addr 0x%08x don't align/* Need 32B alignment*/\n",ram);
        return -1;
    }
    if(len < 0 || len > chipsize)
    {
        printf("the LEN 0x%08x(%d) is a unvalid number\n",len,len);
        return -1;
    }
    return 0;
}

int read_nand_nocheck(u32 ram,u32 flash,u32 len,u8 flag)
{
    u32 page=0;
    u32 ret = 0;
    u32 pages = 0, chunkpage=0;
    page = FLASH_TO_PAGE(flash);//start page 
    switch(flag){
        case 0:
            chunkpage = 0x800;
            break;
        case 1:
            chunkpage = 0x40;
            break;
        case 2:
            chunkpage = 0x840;
            break;
        default:
            chunkpage = 0;
            break;
    }    
    printf("\nreading ");
    pages = (len + chunkpage - 1)/chunkpage;
        ret = read_pages(ram,page,pages,flag);
        printf(". done\n");
return ret;
}

int write_nand_nocheck(u32 ram,u32 flash,u32 len,u8 flag)
{
    u32 page=0;
    u32 ret = 0;
    u32 pages = 0, chunkpage=0;
    ret = error_check(ram,flash,len);
    if(ret)
        return ret;
    page = FLASH_TO_PAGE(flash);//start page 
    switch(flag){
        case 0:
            chunkpage = 0x800;
            break;
        case 1:
            chunkpage = 0x40;
            break;
        case 2:
            chunkpage = 0x840;
            break;
        default:
            chunkpage = 0;
            break;
    }
    printf("\nwriting. ");    
    pages = (len + chunkpage - 1)/chunkpage;
        ret = write_pages(ram ,page, pages, flag);
        printf(". done\n");
    return ret;
}

int read_nand(u32 ram,u32 flash,u32 len,u8 flag)
{
    u32 page=0;
    u32 ret = 0;
    u32 b_pages = 0,a_pages=0;
    u32 pages = 0,block=0,chunkblock=0,chunkpage=0;
    ret = error_check(ram,flash,len);
    if(ret)
        return ret;
    page = FLASH_TO_PAGE(flash);//start page 
//        printf("%x:page==0x%08x\n",__LINE__,page);
    while(is_bad_block(PAGE_TO_BLOCK(page)))
    {
//        printf("%x:page==0x%08x\n",__LINE__,page);
//        printf("%x:block==0x%08x\n",__LINE__,PAGE_TO_BLOCK(page));
//        printf("block==%x,page==%x\n",PAGE_TO_BLOCK(page),page);
//        MYDBG;
        page+=PAGES_A_BLOCK;
    
        page = (BLOCK_TO_PAGE(PAGE_TO_BLOCK(page)));
//        printf("%x:page==0x%08x\n",__LINE__,page);
        printf("the FLASH addr 0x%08x is a bad block,auto change FLASH addr 0x%08x\n",flash,PAGE_TO_FLASH(page));
    }
    switch(flag){
        case 0:
            chunkblock = 0x20000;
            chunkpage = 0x800;
            break;
        case 1:
            chunkblock = 0x1000;
            chunkpage = 0x40;
            break;
        case 2:
            chunkblock = 0x21000;
            chunkpage = 0x840;
            break;
        default:
            chunkblock = 0;
            chunkpage = 0;
            break;
    }    
    printf("\nreading ");
    pages = (len + chunkpage - 1)/chunkpage;
//    printf("%x:pages==0x%08x\n",__LINE__,pages);
    b_pages = page % PAGES_A_BLOCK;
    b_pages = (PAGES_A_BLOCK - b_pages);
    if(pages > b_pages && b_pages > 0){
        ret = read_pages(ram,page,b_pages,flag);
        pages -= b_pages;
        page += b_pages;
        ram += (chunkpage * b_pages); 
    }else if(pages <= b_pages && b_pages > 0){
        ret = read_pages(ram,page,pages,flag);
        pages = 0;
    }
    if(ret){return ret;}
    if(pages){
        block = pages / PAGES_A_BLOCK;
        a_pages = pages % PAGES_A_BLOCK;
    }
    for(;block > 0;block--){
        while(is_bad_block(PAGE_TO_BLOCK(page))){
            printf("the FLASH addr 0x%08x is bad block,try next block...\n",PAGE_TO_FLASH(page));
            page += PAGES_A_BLOCK;
        }
        ret = read_pages(ram,page,PAGES_A_BLOCK,flag);
        printf(". ");
        if(ret) {return ret;}
        pages -= PAGES_A_BLOCK;
        page += PAGES_A_BLOCK ;
        ram += chunkblock;
    } 
    if(a_pages){
        while(is_bad_block(PAGE_TO_BLOCK(page))){
            printf("the FLASH addr 0x%08x is bad block,try next block...\n",PAGE_TO_FLASH(page));
            page += PAGES_A_BLOCK;
        }
        ret = read_pages(ram,page,a_pages,flag);
        if(ret){return ret;}
    }
        printf(". done\n");
    return ret;
}
int write_nand(u32 ram,u32 flash,u32 len,u8 flag)
{
    u32 page=0,start_page=0;
    u32 ret = 0;
    u32 b_pages = 0,a_pages=0;
    u32 pages = 0,block=0,chunkblock=0,chunkpage=0;
    ret = error_check(ram,flash,len);
    if(ret)
        return ret;
    page = FLASH_TO_PAGE(flash);//start page 
    while(is_bad_block(PAGE_TO_BLOCK(page)))
    {
        page+=PAGES_A_BLOCK;

        page = (BLOCK_TO_PAGE(PAGE_TO_BLOCK(page)));
        printf("the FLASH addr 0x%08x is a bad block,auto change FLASH addr 0x%08x\n",flash,PAGE_TO_FLASH(page));
    }
    switch(flag){
        case 0:
            chunkblock = 0x20000;
            chunkpage = 0x800;
            break;
        case 1:
            chunkblock = 0x1000;
            chunkpage = 0x40;
            break;
        case 2:
            chunkblock = 0x21000;
            chunkpage = 0x840;
            break;
        default:
            chunkblock = 0;
            chunkpage = 0;
            break;
    }
    printf("\nwriting. ");    
    pages = (len + chunkpage - 1)/chunkpage;
//    printf("%x:pages==0x%08x\n",__LINE__,pages);
    start_page = page % PAGES_A_BLOCK;
    if(start_page){
        b_pages = (PAGES_A_BLOCK - start_page);
    }
    if(b_pages > 0){
        ret = read_pages(NAND_TMP,page - start_page,start_page,2);
        if(ret){return ret;}
        erase_block(PAGE_TO_BLOCK(page));
        ret = write_pages(NAND_TMP,page - start_page,start_page,2);
        printf(". ");
        if(ret){return ret;}
    }
    if(pages > b_pages && b_pages > 0){
        ret = write_pages(ram,page,b_pages,flag);
        pages -= b_pages;
        page += b_pages;
        ram += (chunkpage * b_pages); 
    }else if(pages <= b_pages && b_pages > 0){
        ret = write_pages(ram,page,pages,flag);
        pages = 0;
    }
    if(ret){return ret;}
    if(pages){
        block = pages / PAGES_A_BLOCK;
        a_pages = pages % PAGES_A_BLOCK;
    }
 //   printf("block==0x%08x,a_pages==0x%08x\n",block,a_pages);
        //MYDBG;
    for(;block > 0;block--){
        //MYDBG;
        erase_block(PAGE_TO_BLOCK(page));
        while(is_bad_block(PAGE_TO_BLOCK(page))){
            printf("the FLASH addr 0x%08x is bad block,try next block...\n",PAGE_TO_FLASH(page));
            page += PAGES_A_BLOCK;
            erase_block(PAGE_TO_BLOCK(page));
        }
        //MYDBG;
        ret = write_pages(ram,page,PAGES_A_BLOCK,flag);
        //MYDBG;
        printf(". ");
        if(ret) {return ret;}
        pages -= PAGES_A_BLOCK;
        page += PAGES_A_BLOCK ;
        ram += chunkblock;
    } 
        //MYDBG;
    if(a_pages){
        erase_block(PAGE_TO_BLOCK(page));
        while(is_bad_block(PAGE_TO_BLOCK(page))){
            printf("the FLASH addr 0x%08x is bad block,try next block...\n",PAGE_TO_FLASH(page));
            page += PAGES_A_BLOCK;
            erase_block(PAGE_TO_BLOCK(page));
        }
        ret = write_pages(ram,page,a_pages,flag);
        if(ret){return ret;}
    }
        printf(". done\n");
    return ret;
}

int erase_nand(u32 flashaddr,u32 len)
{
    int i=0;
    u32 start, blocks;
    start = flashaddr>>17;
    blocks = (len + (1<<17)-1)>>17;
    printf("\nerasing ");
    for(;i<blocks;i++){
        erase_block(i+start);
        printf(". ");
    }
        printf(" done\n");
    //MYDBG
    return 0;
}

int identify_nand()
{
    unsigned int id_val_l=0,id_val_h=0;

#define  _NAND_IDL      ( *((volatile unsigned int*)(NAND_BASE+IDL)))
#define  _NAND_IDH       (*((volatile unsigned int*)(NAND_BASE+IDH)))
    NAND_WL(CMD,0x21);

    do id_val_h = _NAND_IDH;
    while(!((id_val_l |= _NAND_IDL) & 0xff));
    printf("id is %02x_%08x\n", id_val_h&0xff, id_val_l);

    return 0;
}

int mymain(char *cmd,...)
{
	void **arg;
	void *ap;
	int i;
	__builtin_va_start(ap,cmd);
	arg=ap;
	printf("cmd=%s\n",cmd);
	for(i=0;cmd[i];i++)
	{
		switch(cmd[i]){
			case 'i':
				identify_nand();
				break;
			case 'e':
				erase_nand(*arg++,*arg++);
				printf("erase nand ops OK...\n");
				break;
			case 'R':
				read_nand_nocheck(*arg++,*arg++,*arg++,*arg++);
				printf("read nand ops OK...\n");
				break;
			case 'r':
				read_nand(*arg++,*arg++,*arg++,*arg++);
				printf("read nand ops OK...\n");
				break;
			case 'W':
				write_nand_nocheck(*arg++,*arg++,*arg++,*arg++);
				printf("write nand ops OK...\n");
				break;
			case 'w':
				write_nand(*arg++,*arg++,*arg++,*arg++);
				printf("write nand ops OK...\n");
				break;
			case 'I':
				NAND_WL(CS_RDY_MAP,0x88442200);
				NAND_WL(PARAM,0x400);
				/*mux ctrl0*/
				*(volatile int *)0xbfd00420 = (*(volatile int *)0xbfd00420 & 0x01ffcfff) | 0xea000000;
				*(volatile int *)0xbfd010c4 &= ~(3<<2);
				*(volatile int *)0xbfd010c8 &= ~((0xff<<(68-64))|(3<<(84-64)));;
				break;
			default:
				printf("\ninput invalid OPS(erase_nand;read_nand;write_nand)...\n\n");
				printf("\n\t\t*****Nand Flash OPS*****\n\n");
				{
					printf("Uargs:callbin ./bin/nand_ops.bin e <flashaddr> <len>\n");
					printf("Uargs:callbin ./bin/nand_ops.bin <OPS> <ramaddr> <flashaddr> <len> <flag>\n");
					printf("Uargs:flag: 0=main; 1=spare; 2=spare+main\n\n");
					printf("Uargs:OPS:  w=write nand; r=read nand; e=erase nand; I init nand; i readid;R read nand nocheck;W write nand nocheck\n\n");
				}
				break;
		}
	}

	return 0;
}



