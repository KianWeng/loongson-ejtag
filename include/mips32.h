//
// MIPS32.H
//
// Target specific EJTAG code for MIPS32
//
// Copyright (c) 2002, Jason Riffel - TotalEmbedded LLC.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
//
// Redistributions of source code must retain the above copyright 
// notice, this list of conditions and the following disclaimer. 
//
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in 
// the documentation and/or other materials provided with the
// distribution. 
//
// Neither the name of TotalEmbedded nor the names of its 
// contributors may be used to endorse or promote products derived 
// from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
//

#ifndef dMIPS32_H
#define dMIPS32_H
#include <regdef.h>

// 
// PROTOTYPES
//
int fMIPS32_EJTAG_Machine(int i_socket);
unsigned int fMIPS32_ExecuteDebugModule(unsigned int *pui_module,unsigned int len,unsigned int runonce);
unsigned int context_save(long long *context_reg);
int fix_code(int *codebuf,int size);
unsigned int wait_address(unsigned int address,int write,int waitacc);
unsigned int fMIPS32_USB_DownloadModule(unsigned int *pui_module,unsigned int len);
int goback(unsigned int destaddr);
unsigned int context_restore(long long *context_reg);
char *md5sum(char *buf,int len);

//
// DEFINES
//
// ECR register definition
//
#define dMIPS32_ECR_CPU_RESET_OCCURED  0x00000000 // Set to 1 if a reset occured
#define dMIPS32_ECR_ACCESS_SIZE        0x60000000 // See below note 1
#define dMIPS32_ECR_DOZE               0x00400000 // Set to 1 if processor is in low power mode
#define dMIPS32_ECR_HALT               0x00200000 // Set to 1 if system bus clock is stopped
#define dMIPS32_ECR_PERIPHERAL_RESET   0x00100000 // Set to 1 to reset peripherals (optional)
#define dMIPS32_ECR_ACCESS_RW          0x00080000 // Set to 1 if pending access is write, 0 for read
#define dMIPS32_ECR_ACCESS_PENDING     0x00040000 // Set to 1 to indicate access pending
#define dMIPS32_ECR_CPU_RESET          0x00010000 // Set to 1 to reset CPU (optional)
#define dMIPS32_ECR_PROBE_ENABLE       0x00008000 // Set to 1 to enable remote hosted memory
#define dMIPS32_ECR_PROBE_VECTOR       0x00004000 // Set to 1 to use dmseg location for remote memory
#define dMIPS32_ECR_EJTAG_BREAK        0x00001000 // Set to 1 to request a debug interrupt exception
#define dMIPS32_ECR_IN_DEBUG_MODE      0x00000008 // Set to 1 to indicate in debug mode
#define dMIPS32_ECR_RESET_CPU          0x00010000 // reset cpu

// Note 1
//
// Possible values of dMIPS32_ECR_ACCESS_SIZE
#define dMIPS32_ECR_ACCESS_BYTE        0x00000000 // 0 = 1 byte
#define dMIPS32_ECR_ACCESS_HWORD       0x20000000 // 1 = 2 bytes
#define dMIPS32_ECR_ACCESS_WORD        0x40000000 // 2 = 4 bytes
#define dMIPS32_ECR_ACCESS_TRIPLE      0x60000000 // 3 = ? bytes

// Instruction for return from debug handler
#define dMIPS32_DERET_INSTRUCTION      0x4200001F // Big endian format

// A slight note about EJTAG hosted memory.  Nothing is designated as ROM
// or RAM its all virtual definitions we create for being able to identify
// accesses for instructions or accesses for data.  The begining of EJTAG
// hosted memory is designated for ROM up to where RAM begins.  All 
// virtual registers (RAM) will live above the ROM section.  This is useful
// when exiting debug mode to send the correct DERET instruction only when
// retrieving instructions not data.
//
// Our pseudo stack location
 
 
// Define the memory access registers     
#define dMIPS32_PSEUDO_REG_END            (0xFF200000 + FIFO + 8)  
#define dMIPS32_PSEUDO_FIFO_ADDR          (0xFF200000 + FIFO	)  
#define dMIPS32_PSEUDO_STACK_ADDR         (0xFF200000 + STACK	)
#define dMIPS32_PSEUDO_PARAM_ADDR         (0xFF200000 + PARAM	)
#define dMIPS32_PSEUDO_SERIAL_ADDR        (0xFF200000 + SERIAL	)
#define dMIPS32_PSEUDO_FIFO_ADDR1         (0xFF200000 + FIFO1	)
#define dMIPS32_PSEUDO_RTC_ADDR           (0xFF200000 + RTC		)
#define dMIPS32_PSEUDO_HEX_ADDR		  (0xFF200000 + HEX_ADDR)
#define dMIPS32_PSEUDO_RET_ADDR           (0xFF200000 + RET_ADDR)
#define dMIPS32_DATAMEM_END	              (0xFF200000 + RET_ADDR)
#define dMIPS32_DATAMEM                    0xFF200000

//
// EXTERNS - For sharing the register array
extern unsigned int  ui_ejtag_state;
extern unsigned int  ui_last_signal;
//extern unsigned int aui_writeregisters_code[];
//extern unsigned int aui_readregisters_code[];

extern unsigned int core_cpucount;
extern unsigned int core_cpuno;
extern unsigned int core_cpuwidth;

// Define the register locations of the DRSEG hardware breakpoint registers
// These are not implemented on the au1500 (suck!)
#define dMIPS32_INST_BKPT_STATUS       0xFFFFFFFFFF301000LL
#define dMIPS32_INST_BKPT_ADDR(X)      0xFFFFFFFFFF301100LL + (X * 0x100)
#define dMIPS32_INST_BKPT_ADDR_MSK(X)  0xFFFFFFFFFF301108LL + (X * 0x100)
#define dMIPS32_INST_BKPT_ASID(X)      0xFFFFFFFFFF301110LL + (X * 0x100)
#define dMIPS32_INST_BKPT_CONTROL(X)   0xFFFFFFFFFF301118LL + (X * 0x100)

#define dMIPS32_DATA_BKPT_STATUS       0xFFFFFFFFFF302000LL
#define dMIPS32_DATA_BKPT_ADDR(X)      0xFFFFFFFFFF302100LL + (X * 0x100)
#define dMIPS32_DATA_BKPT_ADDR_MSK(X)  0xFFFFFFFFFF302108LL + (X * 0x100)
#define dMIPS32_DATA_BKPT_ASID(X)      0xFFFFFFFFFF302110LL + (X * 0x100)
#define dMIPS32_DATA_BKPT_CONTROL(X)   0xFFFFFFFFFF302118LL + (X * 0x100)
#define dMIPS32_DATA_BKPT_VALUE(X)     0xFFFFFFFFFF302120LL + (X * 0x100)

// AU1500 supports 1 instruction and 1 data breakpoint
#define dMIPS32_MAX_HARDWARE_BKPTS     0x10

// AU1500 hardware breakpoints put the following value into the
// debug register CP0 #23 sel 0 in the DExcCode bits
#define dMIPS32_AU1500_HW_BKPT         0x17

// States of the debug exception catcher
#define dMIPS32_EJTAG_WAIT_FOR_DEBUG_MODE    0x01
#define dMIPS32_EJTAG_WAIT_FOR_CONTINUE      0x02
#define dMIPS32_EJTAG_EXIT_DEBUG_MODE        0x03
#define dMIPS32_EJTAG_FORCE_DEBUG_MODE       0x04

// Software breakpoint instruction
#define dMIPS32_SW_BREAKPOINT_INSTRUCTION    0x7000003F

// Bits in the debug register (CP0 register 23 select 0)
#define dMIPS32_DEBUG_SW_BKPT                0x00000002
#define dMIPS32_DEBUG_HW_INST_BKPT           0x00000010
#define dMIPS32_DEBUG_HW_DATA_LD_BKPT        0x00000004
#define dMIPS32_DEBUG_HW_DATA_ST_BKPT        0x00000008
#define dMIPS32_DEBUG_HW_DATA_LD_IMP_BKPT    0x00040000
#define dMIPS32_DEBUG_HW_DATA_ST_IMP_BKPT    0x00080000
#define dMIPS32_DEBUG_SINGLE_STEP_BKPT       0x00000001
#define dMIPS32_DEBUG_JTAG_BKPT              0x00000020


//
// EXTERNS - For sharing the register array

#define DRSEG_BASE	0xff300000
/* instruction breakpoint status */
#define IBS_OFFSET	0x1000			
/* instruction breakpoint address n */
#define IBAN_OFFSET(x)	(0x1100+0x100*x)
/* instruction breakpoint address mask n */
#define IBMN_OFFESET(x)	(0x1108+0x100*x)
/* instruction breakpoint asid n */
#define IBASIDN_OFFSET(x)	(0x1110+0x100*x)
/* instruction breakpoint control n */
#define	IBCN_OFFSET(x)	(0x1118+0x100*x)
#define IBS	(DRSEG_BASE+IBS_OFFSET)
#define IBAN(x)		(DRSEG_BASE+IBAN_OFFSET(x))
#define IBMN(x)		(DRSEG_BASE+IBMN_OFFSET(x))
#define IBASIDN(x)	(DRSEG_BASE+IBASIDN_OFFSET(x))
#define IBCN(x)		(DRSEG_BASE+IBCN_OFFSET(x))

/* data breakpoint status */
#define DBS_OFFSET	0x2000
/* data breakpoint address n */
#define DBAN_OFFSET(x)	(0x2100+0x100*x)
/* data breakpoint address mask n */
#define DBMN_OFFSET(x)	(0x2108+0x100*x)
/* data breakpoint asid n */
#define DBASIDN_OFFSET(x)	(0x2110+0x100*x)
/* data breakpoint control n */
#define DBCN_OFFSET(x)	(0x2118+0x100*x)
/* data breakpoint value n */
#define DBVN_OFFSET(x)	(0x2120+0x100*x)
#define DBS	(DRSEG_BASE+DBS_OFFSET)
#define DBAN(x)	(DRSEG_BASE+DBAN_OFFSET(x))
#define DBMN(x)	(DRSEG_BASE+DBMN_OFFSET(x))
#define DBASIDN(x)	(DRSEG_BASE+DBASIDN_OFFSET(x))
#define DBCN(x)	(DRSEG_BASE+DBCN_OFFSET(x))
#define DBVN(x)	(DRSEG_BASE+DBVN_OFFSET(x))
#define HI_HALF_MASK 0x0000000c
#define LO_HALF_MASK 0x00000003
#define min(a,b) ((a)<(b)?(a):(b))
#define tgt_compile(s) tgt_compile(s)
#define tgt_exec(s,len) ({unsigned int codebuf[]=tgt_compile(s);fix_code(codebuf,sizeof(codebuf));fMIPS32_ExecuteDebugModule(codebuf,min(len,sizeof(codebuf)),1);})
#define tgt_exec_flags(s,len,flags) ({unsigned int codebuf[]=tgt_compile(s);fix_code(codebuf,sizeof(codebuf));fMIPS32_ExecuteDebugModule(codebuf,min(len,sizeof(codebuf)),flags);})
#define tgt_exec_safe(s,len) ({unsigned int codebuf[]=tgt_compile(s);fix_code(codebuf,sizeof(codebuf));fMIPS32_ExecuteDebugModule_safe(codebuf,min(len,sizeof(codebuf)),1);})
#define tgt_exec_safe_flags(s,len,flags) ({unsigned int codebuf[]=tgt_compile(s);fix_code(codebuf,sizeof(codebuf));fMIPS32_ExecuteDebugModule_safe(codebuf,min(len,sizeof(codebuf)),flags);})

extern int config_jtag_jrhb;
extern int config_jtag_jalrhb;
extern int config_jtag_jalhb;
extern int config_jtag_synci;


#define LUI(rt,imm) miro_inst_2[pc++] = 0x3c000000|(rt<<16)|((imm)&0xffff)
#define ORI(rt,rs, imm) miro_inst_2[pc++] = 0x34000000|(rs<<21)|(rt<<16)|((imm)&0xffff)
#define DSLL(rd,rt,sa)  miro_inst_2[pc++] =  0x38|(rt<<16)|(rd<<11)|(sa<<6)

#define LI(rt,imm) \
	LUI(rt,((imm)>>16)&0xffff); \
        ORI(rt,rt,((imm))&0xffff);

#define DLI(rt,imm) \
	if(core_cpuwidth == 64) \
	{ \
	LUI(rt,((imm)>>48)&0xffff); \
        ORI(rt,rt,((imm)>>32)&0xffff); \
	DSLL(rt,rt,16); \
        ORI(rt,rt,((imm)>>16)&0xffff); \
	DSLL(rt,rt,16); \
        ORI(rt,rt,(imm)&0xffff); \
	} \
	else \
	{ \
	LUI(rt,((imm)>>16)&0xffff); \
        ORI(rt,rt,(imm)&0xffff); \
	}

#define JR(rs) miro_inst_2[pc++] = 8|(rs<<21)
#define JRHB(rs) miro_inst_2[pc++] = 0x408|(rs<<21)
#define NOP() miro_inst_2[pc++] = 0
#define JALR(rs)  miro_inst_2[pc++] = 9|(rs<<21)|(31<<11)
#define JALRHB(rs)  miro_inst_2[pc++] = 0x409|(rs<<21)|(31<<11)
#define SW(rt, off, base) miro_inst_2[pc++] = 0xac000000|(base<<21)|(rt<<16)|((off))
#define SD(rt, off, base) miro_inst_2[pc++] = ((core_cpuwidth == 64)?0xfc000000:0xac000000)|(base<<21)|(rt<<16)|((off))
#define SYNC() miro_inst_2[pc++] = 0xf
#define SYNCI() miro_inst_2[pc++] = 0x041f0000
#define DMTC0(rt,rd,sel) miro_inst_2[pc++] =((((core_cpuwidth == 64)?0x40a:0x408)<<20)|((rt)<<16)|((rd)<<11)|sel)
#define DERET() miro_inst_2[pc++] = 0x4200001f

#define SYNCX() \
	if(config_jtag_synci) \
		SYNCI(); \
	else \
		SYNC();

#define JRX(rs) \
	if(config_jtag_jrhb) \
		JRHB(rs); \
	else \
		JR(rs);

#define JALRX(rs) \
	if(config_jtag_jalrhb) \
		JALRHB(rs); \
	else \
		JALR(rs);

#endif // #ifndef dMIPS32_H
