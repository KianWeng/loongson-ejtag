#ifndef __MYUSB_H__
#define __MYUSB_H__
#include <errno.h>
#include <signal.h>
#include <usb.h>
#include <string.h>
int set_timer(int ms);
void fMIPS32_ReInit(void);
extern int config_usb_maxtimeout;
#ifndef __CYGWIN__
static inline int myusb_bulk_read(usb_dev_handle *d, int e, char *b, int s, int t)
{
int i;
int t1=t; 
if(t1==0||t1>config_usb_maxtimeout) 
 t1=config_usb_maxtimeout;
 i=usb_bulk_read(d,e,b,s,t1);
 if(i<0){ejtag_sti();log_printf(LOG_ERROR,"bulk read error=%d\n",i);ejtag_cli();set_timer(0);}
 if(i==-ENODEV){fMIPS32_ReInit();} 
 return i;
}

static inline int myusb_bulk_write(usb_dev_handle *d, int e, char *b, int s, int t)
{
int i;
int t1=t; 
if(t1==0||t1>config_usb_maxtimeout) 
 t1=config_usb_maxtimeout;
 i=usb_bulk_write(d,e,b,s,t1);
 if(i<0){ejtag_sti();log_printf(LOG_ERROR,"bulk write error=%d\n",i);ejtag_cli();set_timer(0);}
 if(i==-ENODEV){fMIPS32_ReInit();} 
 return i;
}

#else
static inline int myusb_bulk_read(usb_dev_handle *d, int e, char *b, int s, int t)
{
int i;
int t1=t; 
if(t1==0||t1>config_usb_maxtimeout) 
t1=config_usb_maxtimeout;
i=usb_bulk_read(d,e,b,s,t1);
if(i<0){log_printf(LOG_ERROR,"bulk read error=%d\n",i);
set_timer(0);
}
if(i==-EINVAL||i==-ENODEV||i==-EIO)
{printf("exit\n");jump(-1);} 
return i;
}

static inline int myusb_bulk_write(usb_dev_handle *d, int e, char *b, int s, int t)
{
int i;
int t1=t; 
if(t1==0||t1>config_usb_maxtimeout) 
t1=config_usb_maxtimeout;
i=usb_bulk_write(d,e,b,s,t1);
if(i<0){log_printf(LOG_ERROR,"bulk write error=%d\n",i);
set_timer(0);
}
if(i==-EINVAL||i==-ENODEV||i==-EIO)
{printf("exit\n");jump(-1);} 
return i;
}


#endif

extern char usb_packet[];
extern int usb_packet_size;
extern int usb_mode_sync;
extern usb_dev_handle *current_handle;

static inline int myusb_write_flush()
{
	int i = 0;
	if(usb_packet_size)
	{
		i=myusb_bulk_write(current_handle,2,usb_packet,usb_packet_size,0);
		usb_packet_size = 0;
	}
 return i;
}

static inline int myusb_async_bulk_read(usb_dev_handle *d, int e, char *b, int s, int t)
{
int i;
 if(!usb_mode_sync)
 myusb_write_flush();
 i=myusb_bulk_read(d,e,b,s,t);
 return i;
}

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif
static inline int myusb_async_bulk_write(usb_dev_handle *d, int e, char *b, int s, int t)
{
	int i = 0;
	if(usb_mode_sync)
	{
		i=myusb_bulk_write(d,e,b,s,t);
	}
	else 
	{
		int count;
		do
		{
			if(usb_packet_size||s<512)
			{
				count=min(512-usb_packet_size,s);
				memcpy(usb_packet+usb_packet_size, b, count);
				s -= count;
				usb_packet_size += count;
				b += count;
				i += count;
				if(usb_packet_size==512) myusb_write_flush();
			}
			else if(s>=512)
			{
				i += myusb_bulk_write(d,e,b,s,t);
				break;
			}
		}
		while(s);
	}
	return i;
}

#define usb_bulk_read(d,e,b,s,t) myusb_async_bulk_read(d,e,b,s,t) 
#define usb_bulk_write(d,e,b,s,t) myusb_async_bulk_write(d,e,b,s,t) 
#endif
