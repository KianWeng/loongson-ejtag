#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdint.h>
#define ZLDEBUG
int log_printf(int loglevl,char *fmt,...);
void          fJTAG_Init       (void);
void          fJTAG_unInit     (void);
unsigned char fJTAG_Instruction(unsigned char uc_data_out);
unsigned long long fJTAG_Data (unsigned long long ui_data_out);
unsigned long long fJTAG_Data_width (unsigned long long ui_data_out,int width);
unsigned char jtag_inb(long long addr);
unsigned short jtag_inw(long long addr);
unsigned int jtag_inl(long long addr);
unsigned long long jtag_inq(long long addr);
void jtag_outb(long long addr,unsigned char val);
void jtag_outw(long long addr,unsigned short val);
void jtag_outl(long long addr,unsigned int val);
void jtag_outq(long long addr,unsigned long long val);
void jtag_putmem(char *src,long long addr,unsigned int len);
void jtag_putmem32(char *src,long long addr,unsigned int len);
void jtag_getmem(char *dst,long long addr,unsigned int len);
void gdb_getmem(char *dst,long long addr,unsigned int len);
void gdb_putmem(char *src,long long addr,unsigned int len);
void tgt_getmem(char *dst,long long addr,unsigned int len);
void tgt_putmem(char *src,long long addr,unsigned int len);
int callbin_mem(char *buf,int len,int argc,char **argv);
unsigned char fJTAG_Wiggle(unsigned char uc_tms, unsigned char uc_tdo);
int handle_access(unsigned int ui_address,int ui_control,long long *pui_data,unsigned int start_address,unsigned int end_address,unsigned int *pui_module,int flags);
unsigned long long read_cpu_reg(int regnum);
int write_cpu_reg(int regnum,long long val);
unsigned long long read_cpu_reg_byname(char *p);
unsigned long long read_c0_reg(int reg, int sel);
int write_c0_reg(int reg, int sel, long long val);
unsigned long long symbol_to_addr(char *buf);
int sprintf_symbol(char *buf,unsigned long long address);
void * md_disasm ( char *dest, long long addr, unsigned int instcode);
int cacheflush(unsigned long long start,unsigned int size);
unsigned int fMIPS32_Transfer(char *pui_module,unsigned int len,int rw);
unsigned int fMIPS32_ExecuteInRom(unsigned int *pui_module,unsigned int start_address,unsigned int end_address);
extern unsigned int  ui_datamem[];
extern char *ui_fifomem;
extern char *ui_msg[];
#define ui_fifomem8 ((unsigned long long *)ui_fifomem)
#define ui_fifomem4 ((int *)ui_fifomem)
#define ui_fifomem2 ((short *)ui_fifomem)
#define ui_fifomem1 ((char *)ui_fifomem)
#define ui_fifomem_8(x) (*(unsigned long long *)(ui_fifomem+x))
#define ui_fifomem_4(x) (*(int *)(ui_fifomem+x))
#define ui_fifomem_2(x) (*(short *)(ui_fifomem+x))
#define ui_fifomem_1(x) (*(char *)(ui_fifomem+x))
extern unsigned long long ui_retval;
extern long long  gdb_context_reg[];
extern unsigned int  uififo_raddr,uififo_waddr,ui_fifomem_size;
extern long long ui_stack[];
extern unsigned char ui_stack_index;
extern long long ui_param[];
extern unsigned char ui_param_i,ui_param_o;
extern unsigned int *pmap;
extern unsigned int map_start,map_end;
extern unsigned long long helpaddr;
#define LOG_DEBUG 16
#define LOG_NORMAL 7
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_VERBOSE 1

extern int FD_STDIN;
extern int FD_STDOUT;
extern int FD_STDERR;
#define EXEC_ONCE 1
#define EXEC_NWAITACC 2
#define EXEC_BEFORE 4
#define EXEC_NOFIXINS 8
#define EXEC_NOBREAK 0x10
#define EXEC_NEEDRET 0x20
#define EXEC_NOREBREAK 0x40
#define WAITACC 1
#define WAITDEBUG 2
unsigned int get_acc(unsigned int *op,int *address,long long *data);
void feed_acc(long long *data);
void do_cmd(const char *p);
void do_cmdv(int len,char *fmt,...);
int putmem(char *buf,long long download_addr,int len);
extern int config_core_fpu;
#define dMIPS32_NUM_DEBUG_CORE_REGISTERS    38
#define dMIPS32_NUM_DEBUG_REGISTERS    (38+32+2)
#define MAX_FAKE_REG dMIPS32_NUM_DEBUG_REGISTERS
#define dMIPS32_REALNUM_DEBUG_REGISTERS (config_core_fpu?MAX_FAKE_REG:dMIPS32_NUM_DEBUG_CORE_REGISTERS)

/**
 * Sets @c num bits in @c _buffer, starting at the @c first bit,
 * using the bits in @c value.  This routine fast-paths writes
 * of little-endian, byte-aligned, 64-bit words.
 * @param _buffer The buffer whose bits will be set.
 * @param first The bit offset in @c _buffer to start writing (0-63).
 * @param num The number of bits from @c value to copy (1-64).
 * @param value Up to 64 bits that will be copied to _buffer.
 */
void buf_set_u64(void *_buffer, unsigned first, unsigned num, uint64_t value);
/**
 * Retrieves @c num bits from @c _buffer, starting at the @c first bit,
 * returning the bits in a 64-bit word.  This routine fast-paths reads
 * of little-endian, byte-aligned, 64-bit words.
 * @param _buffer The buffer whose bits will be read.
 * @param first The bit offset in @c _buffer to start reading (0-63).
 * @param num The number of bits from @c _buffer to read (1-64).
 * @returns Up to 32-bits that were read from @c _buffer.
 */
uint64_t buf_get_u64(const void *_buffer, unsigned first, unsigned num);
int ejtag_dr_width(int ir);
int fJTAG_Instructions(int wb, int irbits, unsigned  int *ir ,unsigned int *oldir);
int fJTAG_Datas(int wb, int dr_bits, unsigned int *dr, unsigned int *olddr);
int fJTAG_InstDatas(int wb, unsigned short *ir,unsigned long long *dr, unsigned long long *olddr);
unsigned char fJTAG_Instruction(unsigned char uc_data_out);
unsigned long long fJTAG_Data (unsigned long long ui_data_out);
unsigned long long fJTAG_FastData (unsigned long long ui_data_out);
void fJTAG_Data0 (unsigned long long ui_data_out, unsigned long long *pui_data_in);
void fJTAG_FastData0 (unsigned long long ui_data_out, unsigned long long *pui_data_in);
void fJTAG_collect();
void fJTAG_collect_data_queue(int width, unsigned long long *pdata);
void fJTAG_collect_buf_queue(int width, void *pdata);
void fJTAG_collect_queue(int totalwidth, int bitoff, int width, unsigned long long *pdata);
unsigned short fpga_info_ejtag(unsigned short led_on_flag );
int gdb_accessmem(long long addr,unsigned int len);
int ejtag_cli();
int ejtag_sti();
int ejtag_checkbusy();
int ejtag_setbusy(int flag);
int ejtag_unsetbusy(int flag);
int go(unsigned long long cpubits);
int stop(unsigned long long cpubits);
extern int check_ejtag(char *buf,int *debug_reg, long long *pcur_addr, int *pcur_ins);
int runcmd(char **pcmd);
int runrawcmd(char **pcmd);
int mutex_init();
int mutex_lock();
int mutex_unlock();
extern int runcmd_pid;
#define MAXCPUS 64
int myprintf(const char *fmt, ...);
unsigned long long helpfunc(const char *code, int size);
#define CMD_CPU "\1"
#define CMD_CPU1 "\2"
#define CMD_MEM "\3"
#define CMD_CACHE "\4"
#define CMD_BP "\5"
#define CMD_TRANS "\6"
#define CMD_KERNEL "\7"
#define CMD_FLASH "\10"
#define CMD_CALL "\11"
#define CMD_GDB "\12"
#define CMD_JTAG "\13"
#define CMD_INF "\14"
#define CMD_OTHER "\15"
#define CMD_FUNC "\17"
#endif
