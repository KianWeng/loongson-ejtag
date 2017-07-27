#define SPI_CLIPPERS_WBUF_SIZE	11
#define SPI_CLIPPERS_RBUF_SIZE	4

#define SPI_CLIPPERS_CR			0
#define SPI_CLIPPERS_CM			4
#define SPI_CLIPPERS_CM1		8
#define SPI_CLIPPERS_CM2		12

/*CR register*/
#define SPI_CLIPPERS_CR_CLKPH	0x01	/* SPI clock phase when working */
#define SPI_CLIPPERS_CR_CLKPO	0x02	/* SPI clock active porlarity */
#define SPI_CLIPPERS_CR_SPIEN	0x04	/* SPI controller enable or not */
#define SPI_CLIPPERS_CR_STOP	0x08	/* SPI sequency stop */
#define SPI_CLIPPERS_CR_HOLD	0x10	/* SPI HOLD pin control */
#define SPI_CLIPPERS_CR_DUAL	0x20	/* SPI FLASH dual read mode */
#define SPI_CLIPPERS_CR_SPI2	0x40	/* SPI CS2 for second SPI FLASH */

#define COMMON_INS_WRSR		0x01	/* Common write status instruction */
#define COMMON_INS_PP		0x02	/* Common page program instruction */
#define COMMON_INS_READ		0x03	/* Common read instruction */
#define COMMON_INS_WRDI		0x04	/* Common write disable instruction */
#define COMMON_INS_RDSR		0x05	/* Common read status instruction */
#define COMMON_INS_WREN		0x06	/* Common write enable instruction */
#define COMMON_INS_FARD		0x0B	/* Common fast read instruction */
#define COMMON_INS_RES		0xAB	/* Common release from deep power-down & read device id instruction */
#define COMMON_INS_CP		0xAD	/*Common continuously program mode instruction */
#define COMMON_INS_DP		0xB9	/* Common deep power-down instruction */
#define COMMON_INS_BE		0xD8	/* Common bulk erase instruction */
#define COMMON_INS_SE		0x20	/* Common sector erase instruction */
#define COMMON_INS_2READ	0xBB	/* Common 2x I/O read instruction */
#define COMMON_INS_RDID		0x9F	/* Common read ID instruction */
//#define COMMON_INS_JEDEC	0x9F	/* Common read JEDEC ID instruction */

int spi_flash_program(int *p,char *s,int size);
int spi_flash_device_erase(int *dev_base, int size, int verbose);
int spi_check_sum(char *env_base, size_t size, int set);

#define STO_PROTECT_NUM			16					/* STO protect range number */

static int msk_tab[4] = { 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF };

typedef unsigned int		UINT32;
typedef signed int          INT32;
typedef unsigned short		UINT16;
typedef unsigned char		UINT8;
typedef unsigned long		dma_addr_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;
typedef unsigned int u_int32_t;

int sf_spi_put_data(UINT32 dev_base, UINT32 offset, UINT8 *data, UINT32 len);//lxf   20120313   add
int sf_spi_get_data(UINT32 dev_base, UINT8 *data, UINT32 offset, UINT32 len);//lxf   20120313   add


#define SPI_FLASH_REGBASE	0xbfc00000
//#define DEV_BASE	SPI_FLASH_REGBASE
#define FLASH_PAGE_SIZE	256
#define FLASH_SECTOR_SIZE	4096
#define FLASH_BLOCK_SIZE	524288
#define FLASH_SIZE	0x400000		/*total size of spi flash */

#ifndef HISENSE_SPI_FLASH
#define HISENSE_SPI_FLASH
#endif

#define SUCCESS	0
#define ERR_FAILURE -1

/* IO control command. */
enum sto_ioctrl_commands {
	STO_IOCTRL_DEVICE_ERASE		= 0x1000000,		/* Driver device erase command */
	STO_IOCTRL_SECTOR_ERASE		= 0x2000000,		/* Driver sector erase command */
	STO_IOCTRL_DEVICE_PROTE_ON	= 0x3000000,		/* Driver device write protect command */
	STO_IOCTRL_DEVICE_PROTE_OFF	= 0x4000000,		/* Driver device write protect command */
	STO_IOCTRL_SECTOR_PROTE_ON	= 0x5000000,		/* Driver sector write protect command */
	STO_IOCTRL_SECTOR_PROTE_OFF	= 0x6000000,		/* Driver sector write protect command */
	STO_IOCTRL_SECTOR_SIZE		= 0x7000000,		/* Driver get sector size command */
	STO_IOCTRL_SECTOR_NUMBER	= 0x8000000,		/* Driver get sector number command */
	STO_IOCTRL_KB_ERASE			= 0x9000000,		/* Driver erase certain KB command */
	STO_IOCTRL_OTP_READ			= 0xA000000,		/* Driver read OTP space command */
};


/*  the return of verify    */
#define STO_NORF_ALLSAME	0		/* Flash sector content all same with to-be-writed content when write a sector. */
#define STO_NORF_JUSTWRT	1		/* Flash sector content can be overwrited when write a sector. */
#define STO_NORF_NEEDERA	2		/* Flash sector content need be erased when write sector operation, */

#define CHIP_PROTECT		0


#define SPI_IOCTRL_CR_BITS_SET  	1
#define SPI_IOCTRL_CR_BITS_CLEAR 	2

#define SPI_CLIPPERS_CM_RXEN	0x80

void HAL_PUT_UINT32(volatile UINT32 *addr, UINT32 data);		//lxf   20120313   amend
int HAL_GET_UINT32(volatile UINT32 *addr);

#define osal_task_delay udelay

static int spi_clippers_write(UINT32 dev_base, UINT8 *abuf, int alen, UINT8 *bbuf, int blen)
{
	UINT32 len = alen;
	UINT32 cs = (dev_base & 0x100000 ? SPI_CLIPPERS_CR_SPI2 : 0);	/* 0xBFD00000 for CS2*/
	UINT8 *buf, *wbuf = abuf;
	int cur_len;

//	osal_mutex_lock(locker);
	while (len > 0) {
		cur_len = (len > SPI_CLIPPERS_WBUF_SIZE ? SPI_CLIPPERS_WBUF_SIZE : len);
		len -= cur_len;
		if (len == 0) {
			len = blen;
			buf = bbuf;
			blen = 0;
		} else {
			buf = wbuf + cur_len;
		}

		/* If is the last transmition, set stop register */
		if (len == 0)
			HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CR), cs | SPI_CLIPPERS_CR_SPIEN | SPI_CLIPPERS_CR_STOP);
		else
			HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CR), cs | SPI_CLIPPERS_CR_SPIEN);
		/* Set commands */
		HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM2), (wbuf[10] << 24) | (wbuf[9] << 16) | (wbuf[8] << 8) | wbuf[7]);
		HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM1), (wbuf[6 ] << 24) | (wbuf[5] << 16) | (wbuf[4] << 8) | wbuf[3]);
	    HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM ), (wbuf[2 ] << 24) | (wbuf[1] << 16) | (wbuf[0] << 8) | cur_len);
		wbuf = buf;
	}
//	osal_mutex_unlock(locker);

	return SUCCESS;
}


static int spi_clippers_wread(UINT32 dev_base, UINT8 *wbuf, int wlen, UINT8 *rbuf, int rlen)
{
	UINT32 data;
	UINT32 cs = (dev_base & 0x100000 ? SPI_CLIPPERS_CR_SPI2 : 0);	/* 0xBFD00000 for CS2*/
	int cur_len, i;	/* IC support MAX only 4 bytes continue read */

//	osal_mutex_lock(locker);
	/* First process write data actions */
	while (wlen > 0) {
		cur_len = (wlen > SPI_CLIPPERS_WBUF_SIZE ? SPI_CLIPPERS_WBUF_SIZE : wlen);
		wlen -= cur_len;

		/* If is the last transmition, set stop register */
		if (rlen == 0 && wlen == 0)
			HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CR), cs | SPI_CLIPPERS_CR_SPIEN | SPI_CLIPPERS_CR_STOP);
		else
			HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CR), cs | SPI_CLIPPERS_CR_SPIEN);
		/* Set commands */
		HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM2), (wbuf[10] << 24) | (wbuf[9] << 16) | (wbuf[8] << 8) | wbuf[7]);
		HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM1), (wbuf[6 ] << 24) | (wbuf[5] << 16) | (wbuf[4] << 8) | wbuf[3]);
	    HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM ), (wbuf[2 ] << 24) | (wbuf[1] << 16) | (wbuf[0] << 8) | cur_len);
		wbuf += cur_len;
	}
	/* Then process read data actions */
	while (rlen > 0) {
		cur_len = (rlen > SPI_CLIPPERS_RBUF_SIZE ? SPI_CLIPPERS_RBUF_SIZE : rlen);
		rlen -= cur_len;

		/* If is the last transmition, set stop register */
		if (rlen == 0) {
			HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CR), cs |SPI_CLIPPERS_CR_SPIEN | SPI_CLIPPERS_CR_STOP);
		} else {
			HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CR), cs |SPI_CLIPPERS_CR_SPIEN);
		}
		/* Set transmission commands */
		HAL_PUT_UINT32((UINT32 *)(dev_base + SPI_CLIPPERS_CM ), SPI_CLIPPERS_CM_RXEN | (cur_len << 4) | cur_len);
		/* Read data */
		data = HAL_GET_UINT32((UINT32 *)(dev_base));
		for (i = 0; i < cur_len; i++) {
			*rbuf++ = (UINT8)(data >> (i << 3));
		}
	}
//	osal_mutex_unlock(locker);

	return SUCCESS;
}

/*
static int spi_clippers_io_ctrl(UINT32 dev_base, UINT32 cmd, UINT32 param)
{
	struct spi_clippers_priv *tp = (struct spi_clippers_priv *)(dev->priv);

	switch (cmd) {
	case SPI_IOCTRL_CR_BITS_SET:
		tp->cr |= param;
		break;
	case SPI_IOCTRL_CR_BITS_CLEAR:
		tp->cr &= ~param;
		break;
	default :
		return ERR_FAILURE;
		break;
	}

	return SUCCESS;
}
*/

static void sf_spi_wait_free(UINT32 dev_base, int task_delay)
{
	UINT8 buf;

	/* Read from any address of serial flash to check device free. */
	while (1)
	{
		/* Set RDSR instruction: TX command length is 1, RX data length is 1, total 2 */
		buf = COMMON_INS_RDSR;										/* Command */
		spi_clippers_wread((void *)dev_base, &buf, 1, &buf, 1);
		if ((buf & 1) == 0)
			break;
		if (task_delay)
	       osal_task_delay(1);
	}
}

static void sf_spi_write_enable(UINT32 dev_base, int en)
{
	UINT8 buf;

	if (en) {
		/* Set WREN instruction: TX command length is 1, RX data length is 0, total 1 */
		buf = COMMON_INS_WREN;									/* Command */
	} else {
		/* Set WREN instruction: TX command length is 1, RX data length is 0, total 1 */
		buf = COMMON_INS_WRDI;									/* Command */
	}
	spi_clippers_write(dev_base, &buf, 1, NULL, 0);
}

static void sf_spi_protect_chip(UINT32 dev_base, int en)
{
	UINT8 buf[2];

	buf[0] = COMMON_INS_WRSR;									/* Command */
	sf_spi_write_enable(dev_base, 1);
	if (en) {
		/* Set WRSR instruction: TX command length is 2 (SR=0x7F), RX data length is 0, total 2 */
		buf[1] = 0x3C;//write protect
	} else {
		/* Set WRSR instruction: TX command length is 2 (SR=0x00), RX data length is 0, total 2 */
		buf[1] = 0x03;//write unprotect
	}
	spi_clippers_write(dev_base, &buf, 2, NULL, 0);
       sf_spi_wait_free(dev_base, 0);									/* Wait finish */
}

UINT32 sf_spi_get_id(UINT32 dev_base, UINT8 cmaddr_idx, UINT8 idaddr_idx)
{
	UINT8 cmd = cmaddr_idx;
	UINT8 buf[4];
	UINT32 data;
	UINT32 i;

//	spi_lock((void *)dev_base, 1);
#if CHIP_PROTECT
	sf_spi_protect_chip(dev_base, 1);
#else
	sf_spi_protect_chip(dev_base, 0);
#endif
	/* Set RDID instruction: RDID */
	buf[0] = cmd;												/* Command */
	if (cmd == 0x9F) {		/* JEDEC ID */	//read device identification(RDID)
		/* TX command length is 1, RX data length is 3, total is 4 */
		//spi_wread((void *)dev_base, buf, 1, buf, 3);		/* Send commands */
		spi_clippers_wread((void *)dev_base, buf, 1, buf, 3);		/* Send commands */
		data = (buf[2] << 16) | (buf[1] << 8) | buf[0];
	} else {
		/* TX command length is 4, RX data length is 2, total is 6 */
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		//spi_wread((void *)dev->dev_base, buf, 4, buf, 2);		/* Send commands */
		spi_clippers_wread((void *)dev_base, buf, 4, buf, 2);		/* Send commands */
		data = (buf[1] << 8) | buf[0];
	}
/*	for (i = 0; sto_sf_otp_table[i][0] != 0; i++) {
		if (sto_sf_otp_table[i][0] == (data & 0xFF)) {
			dev_opt_id = i;
			if(i==0)
			{
				if(((data>>16)&0xFF)== 0x16)
					printf("now use MXIC MX25L3205D FLASH\n");
				else
					printf("now use MXIC other FLASH\n");
			}
			else
				printf("now use other manufacturer FLASH\n");
			break;
		}
	}*/
//	spi_lock((void *)dev_base, 0);
	return data;
}

static UINT32 sf_spi_verify(UINT32 dev_base, UINT32 offset, UINT8 *data, UINT32 len)
{
	UINT8 buf[5];
	UINT32 cur_len, src, dst, msk, ret;

	UINT8 cmd =COMMON_INS_READ;
	UINT8 dmlen =0;
	UINT8 crmsk=0xC0;

//	spi_lock((void *)dev_base, 1);
//	spi_clippers_io_ctrl((void *)dev_base, SPI_IOCTRL_CR_BITS_SET, crmsk);
	while (len > 0) {
		cur_len = (len > 4 ? 4 : len);
		buf[0] = cmd;											/* Command */
		buf[1] = (UINT8)(offset >> 16);							/* Offset */
		buf[2] = (UINT8)(offset >>  8);
		buf[3] = (UINT8)(offset >>  0);
		buf[4] = 0;												/* Dummy */
		spi_clippers_wread((void *)dev_base, buf, 4 + dmlen, buf, cur_len);	/* Send commands */

		msk = msk_tab[cur_len - 1];
		dst = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
		src = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

		len -= cur_len;
		data += cur_len;
		offset += cur_len;

		dst = (dst ^ src) & msk;
		if (dst == 0)
			continue;
		if (dst & src) {
			/* We will stop read operation for this sector need errase. Return STO_NORF_NEEDERA */
			ret = STO_NORF_NEEDERA;
			break;
		} else {
			ret = STO_NORF_JUSTWRT;
		}
	}
//	spi_clippers_io_ctrl((void *)dev_base, SPI_IOCTRL_CR_BITS_CLEAR, crmsk);
	sf_spi_wait_free(dev_base, 0);									/* Wait finish */
//	spi_lock((void *)dev_base, 0);
	return ret;
}

static int sf_spi_era_chip(UINT32 dev_base)
{
	UINT8 buf;

//	spi_lock((void *)dev_base, 1);
#if CHIP_PROTECT
	/* Un-protect chip */
	sf_spi_protect_chip(dev_base, 0);
#endif
	/* Enable write */
	sf_spi_write_enable(dev_base, 1);								/* Enable write */
	/* Set errase instruction: TX command length is 1, RX data length is 0, total 1 */
	buf = COMMON_INS_BE;										/* Command */
	spi_clippers_write(dev_base, &buf, 1, NULL, 0);
    sf_spi_wait_free(dev_base, 1);							/* Wait finish */
#if CHIP_PROTECT
	/* Protect chip */
	sf_spi_protect_chip(dev_base, 1);
#endif
//	spi_lock((void *)dev_base, 0);
    return SUCCESS;
}

static int sf_spi_era_sect(UINT32 dev_base, UINT32 offset)
{
	UINT8 buf[4];

//	spi_lock((void *)dev_base, 1);
#if CHIP_PROTECT
	/* Un-protect chip */
	sf_spi_protect_chip(dev_base, 0);
#endif
	sf_spi_write_enable(dev_base, 1);								/* Enable write */
	/* Set errase instruction: TX command length is 4, RX data length is 0, total 4 */
	buf[0] = COMMON_INS_SE;										/* Command */
	buf[1] = (UINT8)(offset >> 16);								/* Offset */
	buf[2] = (UINT8)(offset >>  8);
	buf[3] = (UINT8)(offset >>  0);
	spi_clippers_write((void *)dev_base, buf, 4, NULL, 0);			/* Send commands */
       sf_spi_wait_free(dev_base, 1);									/* Wait finish */
#if CHIP_PROTECT
	/* Protect chip */
	sf_spi_protect_chip(dev_base, 1);
#endif
//	spi_lock((void *)dev_base, 0);
    return SUCCESS;
}

int sf_spi_put_data(UINT32 dev_base, UINT32 offset, UINT8 *data, UINT32 len)
{
	UINT32 plen =FLASH_PAGE_SIZE;
	UINT8 buf[5];
	UINT32 cur_len;

//	spi_lock((void *)dev_base, 1);
#if CHIP_PROTECT
	/* Un-protect chip */
	sf_spi_protect_chip(dev_base, 0);
#endif
    while (len > 0) {
		cur_len = ((offset + plen) & ~(plen - 1)) - offset;		/* Num to align */
		if (cur_len > len)
			cur_len = len;
		sf_spi_write_enable(dev_base, 1);							/* Enable write */
		buf[0] = COMMON_INS_PP;									/* Command */
		buf[1] = (UINT8)(offset >> 16);							/* Offset */
		buf[2] = (UINT8)(offset >>  8);
		buf[3] = (UINT8)(offset >>  0);
		spi_clippers_write((void *)dev_base, buf, 4, data, cur_len);/* Send commands */
		len -= cur_len;
		data += cur_len;
		offset += cur_len;
		sf_spi_wait_free(dev_base, 0);								/* Wait finish */
    }
#if CHIP_PROTECT
	/* Protect chip */
	sf_spi_protect_chip(dev_base, 1);
#endif
//	spi_lock((void *)dev_base, 0);
	return SUCCESS;
}

int sf_spi_get_data(UINT32 dev_base, UINT8 *data, UINT32 offset, UINT32 len)
{
	UINT8 buf[5];

	UINT8 cmd =COMMON_INS_READ;
	UINT8 dmlen =0;
	UINT8 crmsk = 0xC0;

//	spi_lock((void *)dev_base, 1);
//	spi_clippers_io_ctrl((void *)dev_base, SPI_IOCTRL_CR_BITS_SET, crmsk);
	buf[0] = cmd;												/* Command */
	buf[1] = (UINT8)(offset >> 16);								/* Offset */
	buf[2] = (UINT8)(offset >>  8);
	buf[3] = (UINT8)(offset >>  0);
	buf[4] = 0;												/* Dummy */
	spi_clippers_wread((void *)dev_base, buf, 4+ dmlen, data, len);/* Send commands */
//	spi_clippers_io_ctrl((void *)dev_base, SPI_IOCTRL_CR_BITS_CLEAR, crmsk);
	sf_spi_wait_free(dev_base, 0);									/* Wait finish */
//	spi_lock((void *)dev_base, 0);
	return SUCCESS;
}



static int sf_spi_write_flash(UINT32 dev_regbase, UINT32 offset, UINT8 *data, UINT32 len)
{
	int erase_len=len;
	int ret,count;
	int erase_offset= offset;
	UINT8 *rbuf;
	int allign,i;
	int wlen,retval=0;

	allign = offset-(offset&~0xFF);

	erase_offset = erase_offset & ~(FLASH_SECTOR_SIZE-1);

	//erase by sector
	while(erase_len > 0)
	{
		sf_spi_era_sect(SPI_FLASH_REGBASE,erase_offset);
		erase_len -=FLASH_SECTOR_SIZE;
		erase_offset +=FLASH_SECTOR_SIZE;
	}

	//printf("******* allign = 0x%x, len = %d \n", allign,len);

	if (allign+len>FLASH_PAGE_SIZE)
	{
	/*	if((offset !=0)&allign!=0)
		{
			sf_spi_get_data(dev_regbase,rbuf,offset&~0xFF,allign);
			for (i=0;i<allign;i++)
				wbuf[i]=*rbuf++;
			for(i=0;i<len;i++)
				wbuf[i+allign]=*data++;
		}//allign the begin of the page		*/
		//len=len+allign;
		sf_spi_put_data(dev_regbase,offset, data,FLASH_PAGE_SIZE-allign);
		ret=sf_spi_verify(dev_regbase,offset, data, FLASH_PAGE_SIZE-allign);//verify
		if (ret == STO_NORF_NEEDERA)
		{
			printf("this sector need errase.\n");
		}
		else
		{
			//printf("STO_NORF_JUSTWRT.\n");
		}
		len=len-(FLASH_PAGE_SIZE-allign);
		offset=offset+(FLASH_PAGE_SIZE-allign);
		data=data+(FLASH_PAGE_SIZE-allign);
		while(len>0)
		{
			wlen=len>FLASH_PAGE_SIZE?FLASH_PAGE_SIZE:len;
			ret = sf_spi_put_data(dev_regbase,offset, data,wlen);//write
			if (ret != SUCCESS )
			{
				printf("program not success\n");
				retval = -1;
				break;
			}
			else
			{
				ret=sf_spi_verify(dev_regbase,offset, data, wlen);//verify
				if (ret == STO_NORF_NEEDERA)
				{
					printf("this sector need errase.\n");
					retval = -2;
					break;
				}
			}
			len=len-wlen;
			data=data+wlen;
			offset=offset+wlen;//move point to correct position.
		}
	}
/***********************lxf   20120314   add   begin *****************************/
	else
	{
		sf_spi_put_data(dev_regbase,offset, data,len);
		ret=sf_spi_verify(dev_regbase,offset, data, len);//verify

	}
/***********************lxf   20120314   add   end *****************************/
	return retval;
	
}


int spi_flash_program(int *p, char *s,int size)
{
        int dev_base=SPI_FLASH_REGBASE;
        int offset;
        int data;
        offset =(int)p-dev_base;

		if((size <=0)||(p<dev_base))
		{
			printf("Bad parameters\n");
			return(-1);             /* Bad parameters */
		}
		else if(((UINT32)p +(UINT32)size)>((UINT32)dev_base+(UINT32)FLASH_SIZE))
		{
			printf("over flow the flash!\n");
			return (-1);
		}
		else
		{
			sf_spi_write_enable(dev_base, 1);
			data = sf_spi_get_id(dev_base, COMMON_INS_RDID, NULL);
			//printf("get id = %x \n",data);
			sf_spi_write_flash(dev_base,offset,s,size);
		}
        return SUCCESS;
}

int spi_flash_device_erase(int *dev_base, int size, int verbose)
{
	int data;
	int offset;
	int ret = 0;

	offset=dev_base-SPI_FLASH_REGBASE;
	sf_spi_write_enable(dev_base,1);
	data = sf_spi_get_id(dev_base, COMMON_INS_RDID, NULL);
	//printf("get id = %x\n",data);
	if ((dev_base < SPI_FLASH_REGBASE)||(dev_base+size >SPI_FLASH_REGBASE+FLASH_SIZE)||(size >FLASH_SIZE))
	{
		printf("over the flash area!\n");
		return(-2);
	}
	if ((size ==-1)&&(dev_base==SPI_FLASH_REGBASE))
	{
		printf("now erasing the full flash!\n");
		ret = sf_spi_era_chip(dev_base);
		if(ret == SUCCESS)
			printf("erase flash success \n");
		else
			printf("erase flash failed \n");
	}
	else
	{
		printf("now erasing some parts in flash!\n");
		while(size > 0)
		{
			sf_spi_era_sect(SPI_FLASH_REGBASE,offset);
			size -=FLASH_SECTOR_SIZE;
			offset +=FLASH_SECTOR_SIZE;
		}
	}
	sf_spi_write_enable(dev_base,0);
	return 0;
}

int spi_check_sum(char *env_base, size_t size, int set)
{
	u_int16_t sum = 0;
	u_int8_t *sp;
	int sz = size / 2;
	sf_spi_get_data(SPI_FLASH_REGBASE,sp,env_base-SPI_FLASH_REGBASE,size);

	if(set) {
		*sp = 0;	/* Clear checksum */
		*(sp+1) = 0;	/* Clear checksum */
	}
	while(sz--) {
		sum += (*sp++) << 8;
		sum += *sp++;
	}
	if(set) {
		sum = -sum;
		sf_spi_write_flash(SPI_FLASH_REGBASE,env_base-SPI_FLASH_REGBASE,sum,2);
	//	*(u_int8_t *)p = sum >> 8;
	//	*((u_int8_t *)p+1) = sum;
	}
	return(sum);
}

/**********lxf   20120313   add   begin********************/
void HAL_PUT_UINT32(volatile UINT32 *addr, UINT32 data)
{
    if ((UINT32)addr < 0x80000000) {
        addr = (UINT32 *)((UINT32)addr + 0xA0000000);
    }
        *addr = data;
}

int HAL_GET_UINT32(volatile UINT32 *addr)
{
    if ((UINT32)addr < 0x80000000) {
        addr = (UINT32 *)((UINT32)addr + 0xA0000000);
    }
        return *addr;
}


int mymain(int flashaddr,unsigned long memaddr,int size, char *incmd)
{
	int argc = *(int *)ARGC_REG;
	char *cmd = "p";

    if(argc < 3)
    { 
        printf("missing args: flashaddr memaddr size [spibase]\n");
	return -1;
    }
   
    if(argc >= 4)
       cmd = incmd;

    printf("flashaddr=0x%x,mem=0x%lx,size=0x%x \n",flashaddr,memaddr,size);
    for(;*cmd;cmd++)
    {
	    switch(*cmd)
	    {
		    case 'e':
			    printf("\n\nerase flash area");
			    spi_flash_device_erase(flashaddr,size,1);
			    break;
		    case 'p':
			    printf("\n\nwrite BIOS to flash\n");
			    spi_flash_program(flashaddr,memaddr,size);
			    break;
	    }

    }
    printf("\n");
    return 0;
}

