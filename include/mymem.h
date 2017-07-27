#ifndef __MYMEM_H__
#define __MYMEM_H__
union commondata{
		unsigned char data1;
		unsigned short data2;
		unsigned int data4;
		unsigned int data8[2];
		unsigned char c[8];
};

int (*syscall1)(int type,long long addr,union commondata *mydata);
int (*syscall2)(int type,long long addr,union commondata *mydata);
extern int syscall_addrwidth;
#endif
