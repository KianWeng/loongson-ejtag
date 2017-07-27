/* this program used to download ls1c executable .bin file mostly,the ram space
 * allocation as follows:
 * 0xa000_0000 --- 0xa008_0000  the ejtag helpaddr & bootparam_addr
 * 0xa008_1000					dma desc addr
 * 0xa010_0000 --- 0xa080_0000  the 8M space used to copy data
 *
 * Support Option: read id(i), read(r), write(w), erase(e)
 *
 * CMD:
 * callbin bin/nand_ops_ls1c.bin <option> <param,...>
 *
 * APPLICATION EXAMPLE:
 * READ ID:
 * option: i
 * cpu0 -callbin bin/nand_ops_ls1c.bin i
 * cmd=i
 * id is c2_f1801dc2
 *******************************************************
 * ERASE NAND:
 * Option:	e
 * Param:	flash_addr size
 * cpu0 -callbin bin/nand_ops_ls1c.bin e 0 0x100000
 * cmd=e
 * erasing . . . . . . . .  done
 *******************************************************
 * READ NAND:
 * Option:	r
 * Param;	ram_addr flash_addr size option_area(0:mian, 1:spare, 2:all)
 * cpu0 -callbin bin/nand_ops_ls1c.bin r 0xa0200000 0 0x20000 0
 * cmd=r
 *
 * reading . done
 * cpu0 -d4 0xa0200000 0x10
 * a0200000: 40806000 40806800 3c080040 40886000 .`.@.h.@@..<.`.@
 * a0200010: 3c1d8100 27bdc000 3c1c8106 279c4000 ...<...'...<.@.'
 * a0200020: 3c08bfe7 35088030 24090002 ad090004 ...<0..5...$....
 * a0200030: 3c098000 3529600c ad090000 34098203 ...<.`)5.......4
 *******************************************************
 *
 * WRITE NAND:
 * Option:	w
 * Param:	ram_addr flash_addr size option_area
 * cpu0 -callbin bin/nand_ops_ls1c.bin w 0x81000000 0 376848 0
 * cmd=w
 *
 * writing. . . . done
 *
 */

/************************* MACRO DEFINITIONS ********************/
#define __ww(addr,val)  *((volatile unsigned int*)(addr)) = (val)
#define __rw(addr,val)  val =  *((volatile unsigned int*)(addr))

#define _RL(x,y)    ((y) = *((volatile unsigned int*)(x)))
/***************************** NAND **************************/
#define	NAND_SIZE			0x08000000	/*128M Bytes*/
#define PAGES_A_BLOCK       0x40

#define BLOCK_TO_PAGE(x)  ((x)<<6)
#define PAGE_TO_BLOCK(x)  ((x)>>6)
#define PAGE_TO_FLASH(x)  ((x)<<11)
#define PAGE_TO_FLASH_H(x)  ((x)>>21)
#define PAGE_TO_FLASH_S(x)  ((x)<<12)
#define PAGE_TO_FLASH_H_S(x)  ((x)>>20)
#define FLASH_TO_PAGE(x)  ((x)>>11)

#define NAND_REG_BASE		0xbfe78000
#define NAND_DMA_BASE        0x1fe78040

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

#define ADDR_C              ADDRL
#define ADDR_R              ADDRH

#define NFLAG_MAIN				0
#define NFLAG_SPARE				1
#define NFLAG_ALL				2

#define CMD_DEV_TO_RAM			(0<<12)
#define CMD_RAM_TO_DMA			(1<<12)

#define NAND_WL(x,y)    __WL((x) + NAND_REG_BASE,(y))
#define NAND_RL(x,y)    __RL((x) + NAND_REG_BASE,(y))

#define STATUS_TIME 100
#define cmd_to_zero  do{            \
            __WL(NAND_REG_BASE,0);  \
            __WL(NAND_REG_BASE,0);  \
            __WL(NAND_REG_BASE,0x400);  \
}while(0)

/************************** DMA & RAM *********************/
#define DMA_ORDER           0x0
#define DMA_SADDR           0x4
#define DMA_DEV             0x8
#define DMA_LEN             0xc
#define DMA_SET_LEN         0x10
#define DMA_TIMES           0x14
#define DMA_CMD             0x18

#define STOP_DMA            (__WL(0xbfd01160,0x10))
#define START_DMA(x)       do{__WL(0xbfd01160,(VT_TO_PHY(x)|0x8));}while(0)
#define ASK_DMA(x)          (__WL(0xbfd01160,(VT_TO_PHY(x)|0x4)))

#define DMA_DESP0				0xa00c0000
#if 0
#define DMA_DESP				0xa0800000
#define DMA_DESP_ORDER			0x00800008
#define DMA_ASK_ORDER			0x00800004
#endif
/*擦除nandflash后，进行校验时，把nandflash擦出区域1块的数据 读到该内存地址，以验证是否为0xff*/
/*write nand  used ,small than 1 block 128k space*/
/*test bad block used ,2 pages =4k space*/
#define RAM_BUF_ADDR				0xa0100000
//#define RAM_BUF_ADDR				0xa2000000

#define DMA_WL(x,y)    __WL((x) + DMA_DESP0,(y))
#define DMA_RL(x,y)    __RL((x) + DMA_DESP0,(y))

/******************************** asm funcs *********************************/
#define en_count(tmp)            do{asm volatile("mfc0 %0,$23;li $2,0x2000000;or $2,%0;mtc0 $2,$23;":"=r"(tmp)::"$2"); }while(0)
#define dis_count(tmp)           do{asm volatile("mtc0 %0,$23;"::"r"(tmp)); }while(0)

/******************************** other funcs *********************************/
#define VT_TO_PHY(x)      ((x) & 0x1fffffff) 
#define __WL(addr,val)    (*((volatile unsigned int*)(addr)) = (val))
#define __RL(addr,val)    ((val) = *((volatile unsigned int*)(addr)))

/********************************* debug funcs ******************************/
#define MYDBG 
//printf("%s:%x\n",__func__,__LINE__)


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

/***************************************************************************
 * Description:
 * Parameters: 
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-17
 ***************************************************************************/

void nand_udelay(unsigned int us)
{
    int tmp=0;
    en_count(tmp);
    udelay(us);
    dis_count(tmp);
}
/***************************************************************************
 * Description:
 * Parameters: 
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-13
 ***************************************************************************/

static void nand_send_cmd(u32 cmd,u32 page)//Send addr and cmd
{
    if((cmd & (0x1<<9)) && !(cmd &(0x1<<8))){  /*如果只读spare 区*/
        NAND_WL(ADDR_C,0x800);
    }else{
        NAND_WL(ADDR_C,0);
    }
    NAND_WL(ADDR_R,page);
    NAND_WL(CMD,cmd & (~0xfe)); /*先清空操作位*/
    NAND_WL(CMD,cmd);			/*执行命令*/
}


static u8 rdy_status(void)
{
	//外部NAND芯片RDY情况
    return ((*((u32*) (NAND_REG_BASE+CMD)) & (0x1<<16)) == 0 ? 0:1);
}
/***************************************************************************
 * Description:  等待nand 操作结束。
 * Parameters: 
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-13
 ***************************************************************************/
#if 0
static u8 nand_op_ok(u32 len,u32 ram)
{
    u32 ask=0,dmalen=0,dmastatus=0;
  //  nand_udelay(1000);
    while(1){
    ASK_DMA(DMA_DESP0);
    _RL(DMA_DESP0+0x4,ask);
    _RL(DMA_DESP0+0xc,dmalen);
    _RL(DMA_DESP0+0x18,dmastatus);
    if(ask == (VT_TO_PHY(ram)+((len+3)/4)*4)){break;} //else{return 0;}
	nand_udelay(60);
    }
    while(!rdy_status())
        nand_udelay(20);
    return 0;
}
#else
static int nand_op_ok(u32 len, u32 ram)
{
	unsigned int cmd,times;
	times = 1000;
    NAND_RL(CMD,cmd);
	while(!(cmd & (0x1<<10))&& times--){
		udelay(100);
		NAND_RL(CMD,cmd);
	}
	if (times)
		return 0;
	else
		return -1;
}

#endif
/***************************************************************************
 * Description: config dma transmission  between ddr and nand
 * Parameters:	len:size  ddr: saddr, cmd: dma option
 * Author  :Sunyoung_yg
 * Date    : 2015-08-13
 ***************************************************************************/

static void dma_config(u32 len,u32 ddr,u32 cmd)
{
    DMA_WL(DMA_ORDER,0);
    DMA_WL(DMA_SADDR,VT_TO_PHY(ddr));
    DMA_WL(DMA_DEV,NAND_DMA_BASE );
    DMA_WL(DMA_LEN,(len+3)/4);
    DMA_WL(DMA_SET_LEN,0);
    DMA_WL(DMA_TIMES,1);
    DMA_WL(DMA_CMD,cmd); 
}
int is_bad_block(u32 block)
{
    u32 page=0;
    page = BLOCK_TO_PAGE(block);
    read_pages(RAM_BUF_ADDR,page,1,1);
   // if(*((u16*) RAM_BUF_ADDR)!= 0xffff){return 1;}
	if (( *(unsigned int*) RAM_BUF_ADDR)&0xffff != 0xffff)
		return 1;
	return 0;
}

/***************************************************************************
 * Description:把1块（128k）数据读到RAM_BUF_ADDR,并校验是否为0xff，若不是，则擦除操作失败
 * Parameters: block_num:	块序号
 * Date    : 2014-02-18
 ***************************************************************************/

int verify_erase(unsigned int block_num)
{
    int i=0;
    unsigned int val=0;
	unsigned int len = 0x20000; /*128k*/
	unsigned int dma_cmd = CMD_DEV_TO_RAM;

	read_pages(RAM_BUF_ADDR, BLOCK_TO_PAGE(block_num),PAGES_A_BLOCK,NFLAG_MAIN);
	/*校验数据*/
    for(i=0; i<0x20000; i=i+4){
        if(*(unsigned int*)(RAM_BUF_ADDR+i) != 0xffffffff){
			printf("\naddr 0x%08x data: 0x%08x isn't 0xFFFFFFFF\r\n",(RAM_BUF_ADDR+i),*(unsigned int*)(RAM_BUF_ADDR+i));
			return -1;
		}
    }
    return 0;
}


/***************************************************************************
 * Description:  擦出NANDFLASH中的第block_num 块。
 * Parameters: block_num: NANDFLASH 中的 第 block_num 块
 * Author  :Sunyoung_yg 
 * Date    : 2014-02-18
 ***************************************************************************/

int erase_block(u32 block_num)
{
	int status_time, ret=0;
	unsigned int nand_cmd = 0x9, tmp;

//	__ww(NAND_REG_BASE+0x0, 0x0);	//clear
//	__ww(NAND_REG_BASE+0x0, 41);	//reset

//	nand_udelay(5000);
	__ww(NAND_REG_BASE+0x4, 0);		 //页内地址
	printf(" block num is :%d ...\r\n", block_num);
	__ww(NAND_REG_BASE+0xc, 0x209);
	__ww(NAND_REG_BASE+0x1c, 1);		// one block
	__ww(NAND_REG_BASE+0x8, block_num << 6);	//128K

	__ww(NAND_REG_BASE+0x0, 0x0);		//erase option
	__ww(NAND_REG_BASE+0x0, 0x9);		//effective

	nand_udelay(50);
#if 0
	udelay(2000);
	status_time = STATUS_TIME+100;
	udelay(2000);
#endif
	while(!rdy_status())
		nand_udelay(30);
	nand_udelay(3000);
	NAND_RL(CMD, tmp);
	printf(" nand option cmd:0x%08x ,..\r\n", tmp);
	ret = verify_erase(block_num);
	if(ret!=0){
		printf(" erase block error!\r\n");
	}

	return ret;
}



/***************************************************************************
 * Description:		从nand 中读取pages 页的数据
 * Parameters: 
 *			ram:	从nand读出的数据写到该地址
 *			page:	要从nand读的起始页号，
 *			pages：	要读的页数
 *			flag:	0==main 1==spare 2==main+spare
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-13
 ***************************************************************************/

int read_pages(u32 ram,u32 page,u32 pages,u8 flag)
{
    u32 len=0,dma_cmd=0,nand_cmd=0;
    u32 ret = 0,tmp;
    switch(flag){
        case NFLAG_MAIN:
            len = pages * 0x800;
            nand_cmd = 0x103;
            break;
        case NFLAG_SPARE:
            len = pages * 0x40;
            nand_cmd = 0x203;
            break;
        case NFLAG_ALL:
            len = pages * 0x840;
            nand_cmd = 0x303;
            break;
        default:
            len = 0;
            nand_cmd = 0;
            break;
    }
    dma_config(len,ram,dma_cmd);
    STOP_DMA;
    nand_udelay(5);
    START_DMA(DMA_DESP0);

	nand_udelay(10);

	NAND_WL(TIMING,0x412);

//	NAND_WL(TIMING,0x206);
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


int write_pages(u32 ram,u32 page,u32 pages,u8 flag)
{
    u32 len=0,dma_cmd=0x1000,nand_cmd=0;
    u32 ret=0,step=1;
    int val=pages;

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
    for(;val>0;val-=step){
        //printf("len==0x%08x,pages==0x%08x\n",len,val);
        dma_config(len,ram,dma_cmd);
        STOP_DMA;
        nand_udelay(10);
        START_DMA(DMA_DESP0);
        NAND_WL(TIMING,0x412);
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
    if(FLASH_TO_PAGE(flash) >= FLASH_TO_PAGE(NAND_SIZE)){
        printf("the FLASH addr is bigger than NAND_SIZE...\n");
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
    if(len < 0 || len > NAND_SIZE)
    {
        printf("the LEN 0x%08x(%d) is a unvalid number\n",len,len);
        return -1;
    }
    return 0;
}
/***************************************************************************
 * Description:
 * Parameters: 
 *			ram	:	ram addr(Byte), 命令行参数指定 
 *			flash:	flash addr (Byte), 
 *			len:	size;
 *			flag:	0==main 1==spare 2==main+spare
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-13
 ***************************************************************************/

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
/***************************************************************************
 * Description:
 * Parameters: 
 *			ram	:	ram addr(Byte), 命令行参数指定 
 *			flash:	flash addr (Byte), 
 *			len:	size;
 *			flag:	0==main 1==spare 2==main+spare
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-13
 ***************************************************************************/

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
/***************************************************************************
 * Description:	read nand data to ram 
 * Parameters:	
 *				ram  :	the addr will read to
 *				flash:	the addr will read from
 *				len  :  read size(bytes)
 *				flag :  specify read area in nand, 0:NFLAG_MAIN, 1:NFLAG_SPACE,
 *				2:NFALG_ALL;
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-17
 ***************************************************************************/

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
//        MYDBG;
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
    printf("\nreading ");
    pages = (len + chunkpage - 1)/chunkpage;
    //printf("%x:pages==0x%08x\n",__LINE__,pages);
    b_pages = page % PAGES_A_BLOCK;
    b_pages = (PAGES_A_BLOCK - b_pages);
    if(pages > b_pages && b_pages > 0){
        ret = read_pages(ram,page,b_pages,flag);
        pages -= b_pages;
        page += b_pages;
        ram += (chunkpage * b_pages); 
    }else if(pages <= b_pages && b_pages > 0){
    //printf("%x:pages==================0x%08x\n",__LINE__,pages);
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

/***************************************************************************
 * Description:
 * Parameters: ram: ramaddr(bytes);  flash: nandflash addr(bytes); len: (bytes)
 * Author  :Sunyoung_yg 
 * Date    : 2014-02-18
 ***************************************************************************/

int write_nand(u32 ram,u32 flash,u32 len,u8 flag)
{
    u32 page=0,start_page=0;
    u32 ret = 0;
    u32 b_pages = 0,a_pages=0;
    u32 pages = 0,blocks=0,chunkblock=0,chunkpage=0;
    ret = error_check(ram,flash,len);
    if(ret)
        return ret;
    page = FLASH_TO_PAGE(flash);		//start page (bytes addr /2k = page number)
	while(is_bad_block(PAGE_TO_BLOCK(page))) //page / 64 = block number
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

	/*1 block = 64(0x40) pages,start_page is  caculated the offset page of block*/
    start_page = page % PAGES_A_BLOCK;
    if(start_page){
        b_pages = (PAGES_A_BLOCK - start_page);
    }
	/*要操作的页非块对齐，故先把该块中操作页之前的页读出来，擦出整块，然后再重新写回去*/
    if(b_pages > 0){
        ret = read_pages(RAM_BUF_ADDR,page - start_page,start_page,2);
        if(ret){return ret;}
        erase_block(PAGE_TO_BLOCK(page));
        ret = write_pages(RAM_BUF_ADDR,page - start_page,start_page,2);
        printf(". ");
        if(ret){return ret;}
    }
	/*如果要写的总页数大于该块剩余的空闲页数*/
    if(pages > b_pages && b_pages > 0){
        ret = write_pages(ram,page,b_pages,flag);
        pages -= b_pages;
        page += b_pages;
        ram += (chunkpage * b_pages); 
    }
	/*如果要写的总页数小于该块剩余的空闲页数，则写完结束*/
	else if(pages <= b_pages && b_pages > 0){
        ret = write_pages(ram,page,pages,flag);
        pages = 0;
    }
    if(ret){return ret;}
	/*如果还有要写的数据页，则先按块来写，a_pages 是最后的非整块的数据页数*/
    if(pages){
        blocks = pages / PAGES_A_BLOCK;
        a_pages = pages % PAGES_A_BLOCK;
    }
    for(;blocks > 0;blocks--){
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
	/*写最后剩下不足整块的a_pages页*/
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
/***************************************************************************
 * Description	:	erase some blocks
 * Parameters	:	flashaddr: Byte addr;  len: bytes
 * Author		:	Sunyoung_yg
 * Date			:	2014-02-18
 ***************************************************************************/
int erase_nand(u32 flashaddr,u32 len)
{
    int i=0,ret=0;
    u32 start, blocks;
    start = flashaddr>>17;				//"start" is 128k nandflash block addr
    blocks = (len + (1<<17)-1)>>17;     //make ceiling integer (quzheng)
	printf(" blocks is %d ...\r\n", blocks);
    printf("\nerasing ");
    for(i=0; i<blocks; i++){
        ret = erase_block(i+start);
		if(ret){
			printf(" erase_nand error!\r\n");
			return -1;
		}
        printf(". ");
    }
    printf(" done\n");
    return 0;
}
/***************************************************************************
 * Description: read nand id,5 Bytes; idh have one byte, idl have the other 4
 * bytes.
 * Parameters: void
 * Author  :Sunyoung_yg 
 * Date    : 2015-08-13
 ***************************************************************************/

int identify_nand()
{
    unsigned int id_val_l=0,id_val_h=0;
#define  _NAND_IDL      ( *((volatile unsigned int*)(NAND_REG_BASE+IDL)))
#define  _NAND_IDH       (*((volatile unsigned int*)(NAND_REG_BASE+IDH)))
    NAND_WL(CMD,0x21);
    do id_val_h = _NAND_IDH;
    while(!((id_val_l |= _NAND_IDL) & 0xff));
    printf("id is %02x_%08x\n", id_val_h&0xff, id_val_l);
    return 0;
}

typedef unsigned int _u32;
#define WTREG4(addr,val)  *((volatile unsigned int*)(addr)) = (val)

#define RDREG4(addr, val)	((val) = *(volatile _u32 *)(addr))
void nand_init()
{
	_u32  val;
	WTREG4(0xbfe7800c, 0x40a);    //参考手册209页，NAND_TIMING 寄存器， 0x4个周期后，nand命令有效，nand一次读写所需的总时钟周期设置为0x0a
	WTREG4(0xbfe78018, 0x8005000);//nand 1Gb 2k页，id 号共0x5个字节，每次操作的范围0x800（2048）字节，
	WTREG4(0xbfe7801c, 0x800);    //读写操作 2k 字节

	//########################################## read_id
	WTREG4(0xbfe78000, 0x21);	//读ID操作，命令有效
	RDREG4(0xbfe78000, val);   
	while(val&0x400 == 0) {		//等待读完成
		RDREG4(0xbfe78000, val);
	}

	RDREG4(0xbfe78010, val);    //NAND ID 2,3,4,5 Cycle 值
	printf("nand id: 0x%08x...\n\n", val);  //1c target board K9F1G08U0D id: f1009540 

}
/***************************************************************************
 * Description: verify read,write & erase option  program codes.
 * Parameters:
 * Author  :Sunyoung_yg
 * Date    : 2015-09-30
 ***************************************************************************/

int rw_verify(void)
{
	return 0;
}

/***************************************************************************
 * Description: the entry to  option nandflash
 * Parameters:
 * Author  :Sunyoung_yg
 * Date    : 2015-09-30
 ***************************************************************************/

int mymain(char *cmd,...)
{
	void **arg;
	void *ap;
	int i;
	__builtin_va_start(ap,cmd);
	arg=ap;
	printf("cmd=%s\n",cmd);

	nand_init();

	for(i=0;cmd[i];i++)
	{
		switch(cmd[i]){
			case 'i':
				identify_nand();
				break;
			case 'e':
				erase_nand(*arg++,*arg++);
				break;
			case 'R':
				read_nand_nocheck(*arg++,*arg++,*arg++,*arg++);
				break;
			case 'r':
				read_nand(*arg++,*arg++,*arg++,*arg++);
				break;
			case 'W':
				write_nand_nocheck(*arg++,*arg++,*arg++,*arg++);
				break;
			case 'w':
				write_nand(*arg++,*arg++,*arg++,*arg++);
				break;
			case 't':
				rw_verify(); /*verify read,wirte & erase nand options*/
				break;
#if 0
			case 'I':  /*nand_init()  replaced*/
				NAND_WL(CS_RDY_MAP,0x88442200);
				NAND_WL(PARAM,0x400); //??????
				break;
#endif
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



