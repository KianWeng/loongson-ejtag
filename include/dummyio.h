#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parapin.h"

static unsigned long mips_io_port_base;

#define PP_DATA_PORT 0x378
#define PP_STATUS_PORT 0x379

#define dPARPORT_TAP_RESET_BIT     LP_PIN05
#define dPARPORT_TAP_DI_BIT        LP_PIN08
#define dPARPORT_TAP_MS_BIT        LP_PIN03
#define dPARPORT_TAP_CLK_BIT       LP_PIN02
#define dPARPORT_TARGET_RESET_BIT  LP_PIN09
#define dPARPORT_TAP_DO_PIN1        (1<<7)

#define TCK_GPIO (1<<5)
#define TDI_GPIO (1<<2)
#define TDO_GPIO (1<<3)
#define TMS_GPIO (1<<4)
#define TRST_GPIO (1<<15)

#define trst(d) ((d&dPARPORT_TAP_RESET_BIT)?TRST_GPIO:0)
#define tdi(d) ((d&dPARPORT_TAP_DI_BIT)?TDI_GPIO:0)
#define tms(d) ((d&dPARPORT_TAP_MS_BIT)?TMS_GPIO:0)
#define tck(d) ((d&dPARPORT_TAP_CLK_BIT)?TCK_GPIO:0)
#define tdo(d) ((d&TDO_GPIO)?0:dPARPORT_TAP_DO_PIN1) /*invert*/

#define trst_i(d) ((d&TRST_GPIO)?dPARPORT_TAP_RESET_BIT:0)
#define tdi_i(d) ((d&TDI_GPIO)?dPARPORT_TAP_DI_BIT:0)
#define tms_i(d) ((d&TMS_GPIO)?dPARPORT_TAP_MS_BIT:0)
#define tck_i(d) ((d&TCK_GPIO)?dPARPORT_TAP_CLK_BIT:0)


static void initio (void) __attribute__ ((constructor));
static void uninitio (void) __attribute__ ((destructor));
static inline void initio (void)
{
int fd=open("/dev/mem",O_RDWR);
mips_io_port_base=mmap(0, 0x10000, PROT_READ|PROT_WRITE , MAP_SHARED, fd, 0x1fe00000);
if(!mips_io_port_base){printf("error mmap\n");exit(0);}
close(fd);
}

static inline void uninitio (void)
{
munmap(mips_io_port_base,0x4000);
}

static inline int ioperm(unsigned long from, unsigned long num, int turn_on)
{
*(volatile int *)(mips_io_port_base+0x120) = (*(volatile int *)(mips_io_port_base+0x120) & ~(TCK_GPIO|TDI_GPIO|TMS_GPIO|TRST_GPIO))|TDO_GPIO;
return 0;
}



unsigned char __attribute__((noinline))
inb (unsigned short int port)
{
  unsigned char v;
  unsigned int d;
  switch(port)
  {
	  case 0x378:
		  d = *(volatile int *)(mips_io_port_base+0x11c);
                  v = trst_i(d)|tdi_i(d)|tms_i(d)|tck_i(d);
		  break;
	  case 0x379:
		  d = (*(volatile int *)(mips_io_port_base+0x11c)>>16);
		  v = tdo(d); 
		  break;
	  case 0x37a:
		v=0;
		break;
	  default:
		  printf("unknown port 0x%x\n", port);
		  exit(0);
		  break;
  }
  return v;
}


void __attribute__((noinline))
outb (unsigned char v, unsigned short int port)
{
  unsigned int d;
  switch(port)
  {
	  case 0x378:
		  d = v;
		  d = trst(d)|tdi(d)|tms(d)|tck(d);
		  *(volatile unsigned int *)(mips_io_port_base + 0x11c)=d;
		  break;
	  case 0x379:
		  break;
	  case 0x37a:
		break;
	  default:
		  printf("unknown port 0x%x\n", port);
		  exit(0);
		  break;
  }
}

