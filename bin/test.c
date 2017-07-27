/* md5.c - an implementation of the MD5 algorithm and MD5 crypt */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2000, 2001  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* See RFC 1321 for a description of the MD5 algorithm.
 */

#ifdef HAVE_FLOAT
#include <math.h>
#define printf newprintf
#endif

int mymain(char *buf,int c,int len)
{
	int argc = *(int *)ARGC_REG;
#ifdef HAVE_FLOAT
tgt_fpuenable();
#endif
	if(argc==3)
	 printf("buf=0x%x,c=0x%x,len=0x%x\n",buf,c,len);
        else
         printf("test\n");
//	memset(buf,c,len);
	return argc;
}

