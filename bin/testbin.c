#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

char buf[16]={1,2,3,4};
int main()
{
int fd;
char  *(*p)();
unsigned char *pc;
int i;
fd=open("test.bin",O_RDWR);
p=mmap(0,0x100000,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE,fd,0);
close(fd);
printf("p=%p\n",p);
p(buf,3,16);
for(i=0;i<16;i++)
 printf("%02x,",buf[i]);
printf("\n");

return 0;
}

