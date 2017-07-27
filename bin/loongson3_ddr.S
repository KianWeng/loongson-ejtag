/*whd : loongson3_ddr2_config.S
        used to set up all ddr controllers
        and set up the memory space on L2 Xbar
*/

#include "../include/regdef.h"

#define loongson3A3

/*****************************
    Macro defination for DDR MC parameters
    Author: Chen Xinke
    v0.1    
*******************************/
#define DDR_MC_CONFIG_BASE      0x900000000ff00000
#define MC_CONFIG_REG_BASE_ADDR 0x900000000ff00000
#ifdef  loongson3A3
#define DDR_PARAM_NUM   180
#else
#define DDR_PARAM_NUM   152
#endif

#define START_ADDR              (0x30)
#define INT_STATUS_ADDR         (0x960)
#define RESYNC_DLL_ADDR         (0x980)
#define START_OFFSET            40
#define INIT_COMPLETE_OFFSET    8
#define RESYNC_DLL_OFFSET       16

#define CLKLVL_DELAY_2_ADDR      (0x8f0)
#define CLKLVL_DELAY_1_ADDR      (0x8f0)
#define CLKLVL_DELAY_0_ADDR      (0x8f0)
#define CLKLVL_DELAY_2_OFFSET    24
#define CLKLVL_DELAY_1_OFFSET    16
#define CLKLVL_DELAY_0_OFFSET    8

#define PAD_CTRL_REG_ADDR             (0x2d0)
#define PAD_CTRL_COMP_OFFSET          18
#define PAD_CTRL_COMP_MASK            (0xff)

#define PAD_OUTPUT_WINDOW_8_ADDR      (0x310)
#define PAD_OUTPUT_WINDOW_7_ADDR      (0x310)
#define PAD_OUTPUT_WINDOW_6_ADDR      (0x300)
#define PAD_OUTPUT_WINDOW_5_ADDR      (0x300)
#define PAD_OUTPUT_WINDOW_4_ADDR      (0x2f0)
#define PAD_OUTPUT_WINDOW_3_ADDR      (0x2f0)
#define PAD_OUTPUT_WINDOW_2_ADDR      (0x2e0)
#define PAD_OUTPUT_WINDOW_1_ADDR      (0x2e0)
#define PAD_OUTPUT_WINDOW_0_ADDR      (0x2d0)
#define PAD_OUTPUT_WINDOW_8_OFFSET    32
#define PAD_OUTPUT_WINDOW_7_OFFSET    0
#define PAD_OUTPUT_WINDOW_6_OFFSET    32
#define PAD_OUTPUT_WINDOW_5_OFFSET    0
#define PAD_OUTPUT_WINDOW_4_OFFSET    32
#define PAD_OUTPUT_WINDOW_3_OFFSET    0
#define PAD_OUTPUT_WINDOW_2_OFFSET    32
#define PAD_OUTPUT_WINDOW_1_OFFSET    0
#define PAD_OUTPUT_WINDOW_0_OFFSET    32
#define PAD_OUTPUT_WINDOW_MASK     (0xffff)

#define PHY_CTRL_0_8_ADDR        (0x310)
#define PHY_CTRL_0_7_ADDR        (0x310)
#define PHY_CTRL_0_6_ADDR        (0x300)
#define PHY_CTRL_0_5_ADDR        (0x300)
#define PHY_CTRL_0_4_ADDR        (0x2f0)
#define PHY_CTRL_0_3_ADDR        (0x2f0)
#define PHY_CTRL_0_2_ADDR        (0x2e0)
#define PHY_CTRL_0_1_ADDR        (0x2e0)
#define PHY_CTRL_0_0_ADDR        (0x2d0)
#define PHY_CTRL_0_8_OFFSET      32
#define PHY_CTRL_0_7_OFFSET      0
#define PHY_CTRL_0_6_OFFSET      32
#define PHY_CTRL_0_5_OFFSET      0
#define PHY_CTRL_0_4_OFFSET      32
#define PHY_CTRL_0_3_OFFSET      0
#define PHY_CTRL_0_2_OFFSET      32
#define PHY_CTRL_0_1_OFFSET      0
#define PHY_CTRL_0_0_OFFSET      32
#define PHY_CTRL_0_MASK          (0xffffffff)
#define ADD_HALF_CLK_SHIFT       17

#define PHY_CTRL_1_8_ADDR        (0x360)
#define PHY_CTRL_1_7_ADDR        (0x350)
#define PHY_CTRL_1_6_ADDR        (0x350)
#define PHY_CTRL_1_5_ADDR        (0x340)
#define PHY_CTRL_1_4_ADDR        (0x340)
#define PHY_CTRL_1_3_ADDR        (0x330)
#define PHY_CTRL_1_2_ADDR        (0x330)
#define PHY_CTRL_1_1_ADDR        (0x320)
#define PHY_CTRL_1_0_ADDR        (0x320)
#define PHY_CTRL_1_8_OFFSET      0
#define PHY_CTRL_1_7_OFFSET      32
#define PHY_CTRL_1_6_OFFSET      0
#define PHY_CTRL_1_5_OFFSET      32
#define PHY_CTRL_1_4_OFFSET      0
#define PHY_CTRL_1_3_OFFSET      32
#define PHY_CTRL_1_2_OFFSET      0
#define PHY_CTRL_1_1_OFFSET      32
#define PHY_CTRL_1_0_OFFSET      0 
#define PHY_CTRL_1_MASK          (0xffffffff)

#define WRLVL_DQ_DELAY_8_ADDR      (0x230)
#define WRLVL_DQ_DELAY_7_ADDR      (0x230)
#define WRLVL_DQ_DELAY_6_ADDR      (0x220)
#define WRLVL_DQ_DELAY_5_ADDR      (0x220)
#define WRLVL_DQ_DELAY_4_ADDR      (0x210)
#define WRLVL_DQ_DELAY_3_ADDR      (0x210)
#define WRLVL_DQ_DELAY_2_ADDR      (0x200)
#define WRLVL_DQ_DELAY_1_ADDR      (0x200)
#define WRLVL_DQ_DELAY_0_ADDR      (0x1f0)
#define WRLVL_DQ_DELAY_8_OFFSET    48
#define WRLVL_DQ_DELAY_7_OFFSET    16
#define WRLVL_DQ_DELAY_6_OFFSET    48
#define WRLVL_DQ_DELAY_5_OFFSET    16
#define WRLVL_DQ_DELAY_4_OFFSET    48
#define WRLVL_DQ_DELAY_3_OFFSET    16
#define WRLVL_DQ_DELAY_2_OFFSET    48
#define WRLVL_DQ_DELAY_1_OFFSET    16
#define WRLVL_DQ_DELAY_0_OFFSET    48

#define RDLVL_DQSN_DELAY_8_ADDR      (0x280)
#define RDLVL_DQSN_DELAY_7_ADDR      (0x270)
#define RDLVL_DQSN_DELAY_6_ADDR      (0x270)
#define RDLVL_DQSN_DELAY_5_ADDR      (0x260)
#define RDLVL_DQSN_DELAY_4_ADDR      (0x260)
#define RDLVL_DQSN_DELAY_3_ADDR      (0x250)
#define RDLVL_DQSN_DELAY_2_ADDR      (0x250)
#define RDLVL_DQSN_DELAY_1_ADDR      (0x240)
#define RDLVL_DQSN_DELAY_0_ADDR      (0x240)
#define RDLVL_DQSN_DELAY_8_OFFSET    8
#define RDLVL_DQSN_DELAY_7_OFFSET    40
#define RDLVL_DQSN_DELAY_6_OFFSET    8
#define RDLVL_DQSN_DELAY_5_OFFSET    40
#define RDLVL_DQSN_DELAY_4_OFFSET    8
#define RDLVL_DQSN_DELAY_3_OFFSET    40
#define RDLVL_DQSN_DELAY_2_OFFSET    8
#define RDLVL_DQSN_DELAY_1_OFFSET    40
#define RDLVL_DQSN_DELAY_0_OFFSET    8

#ifdef  loongson3A3
#define CLKLVL_DELAY_MASK       (0xff)
#define RDLVL_GATE_DELAY_MASK   (0xffff)
#define RDLVL_DELAY_MASK        (0xffff)
#define RDLVL_DQSN_DELAY_MASK   (0xffff)
#define WRLVL_DELAY_MASK        (0xffff)
#define WRLVL_DQ_DELAY_MASK     (0xffff)

#define WRLVL_DELAY_8_ADDR      (0xb10)
#define WRLVL_DELAY_7_ADDR      (0xb10)
#define WRLVL_DELAY_6_ADDR      (0xb10)
#define WRLVL_DELAY_5_ADDR      (0xb00)
#define WRLVL_DELAY_4_ADDR      (0xb00)
#define WRLVL_DELAY_3_ADDR      (0xb00)
#define WRLVL_DELAY_2_ADDR      (0xb00)
#define WRLVL_DELAY_1_ADDR      (0xaf0)
#define WRLVL_DELAY_0_ADDR      (0xaf0)
#define WRLVL_DELAY_8_OFFSET    32
#define WRLVL_DELAY_7_OFFSET    16
#define WRLVL_DELAY_6_OFFSET    0
#define WRLVL_DELAY_5_OFFSET    48
#define WRLVL_DELAY_4_OFFSET    32
#define WRLVL_DELAY_3_OFFSET    16
#define WRLVL_DELAY_2_OFFSET    0
#define WRLVL_DELAY_1_OFFSET    48
#define WRLVL_DELAY_0_OFFSET    32

#define RDLVL_GATE_DELAY_8_ADDR (0xa90)
#define RDLVL_GATE_DELAY_7_ADDR (0xa90)
#define RDLVL_GATE_DELAY_6_ADDR (0xa90)
#define RDLVL_GATE_DELAY_5_ADDR (0xa90)
#define RDLVL_GATE_DELAY_4_ADDR (0xa80)
#define RDLVL_GATE_DELAY_3_ADDR (0xa80)
#define RDLVL_GATE_DELAY_2_ADDR (0xa80)
#define RDLVL_GATE_DELAY_1_ADDR (0xa80)
#define RDLVL_GATE_DELAY_0_ADDR (0xa70)
#define RDLVL_GATE_DELAY_8_OFFSET    48
#define RDLVL_GATE_DELAY_7_OFFSET    32
#define RDLVL_GATE_DELAY_6_OFFSET    16
#define RDLVL_GATE_DELAY_5_OFFSET    0
#define RDLVL_GATE_DELAY_4_OFFSET    48
#define RDLVL_GATE_DELAY_3_OFFSET    32
#define RDLVL_GATE_DELAY_2_OFFSET    16
#define RDLVL_GATE_DELAY_1_OFFSET    0
#define RDLVL_GATE_DELAY_0_OFFSET    48

#define RDLVL_DELAY_8_ADDR      (0xa50)
#define RDLVL_DELAY_7_ADDR      (0xa50)
#define RDLVL_DELAY_6_ADDR      (0xa40)
#define RDLVL_DELAY_5_ADDR      (0xa40)
#define RDLVL_DELAY_4_ADDR      (0xa40)
#define RDLVL_DELAY_3_ADDR      (0xa40)
#define RDLVL_DELAY_2_ADDR      (0xa30)
#define RDLVL_DELAY_1_ADDR      (0xa30)
#define RDLVL_DELAY_0_ADDR      (0xa30)
#define RDLVL_DELAY_8_OFFSET    16
#define RDLVL_DELAY_7_OFFSET    0
#define RDLVL_DELAY_6_OFFSET    48
#define RDLVL_DELAY_5_OFFSET    32
#define RDLVL_DELAY_4_OFFSET    16
#define RDLVL_DELAY_3_OFFSET    0
#define RDLVL_DELAY_2_OFFSET    48
#define RDLVL_DELAY_1_OFFSET    32
#define RDLVL_DELAY_0_OFFSET    16
#else
#define CLKLVL_DELAY_MASK       (0xff)
#define RDLVL_GATE_DELAY_MASK   (0xff)
#define RDLVL_DELAY_MASK        (0xff)
#define RDLVL_DQSN_DELAY_MASK   (0xff)
#define WRLVL_DELAY_MASK        (0xff)
#define WRLVL_DQ_DELAY_MASK     (0xff)

#define WRLVL_DELAY_8_ADDR      (0x840)
#define WRLVL_DELAY_7_ADDR      (0x840)
#define WRLVL_DELAY_6_ADDR      (0x840)
#define WRLVL_DELAY_5_ADDR      (0x830)
#define WRLVL_DELAY_4_ADDR      (0x830)
#define WRLVL_DELAY_3_ADDR      (0x830)
#define WRLVL_DELAY_2_ADDR      (0x830)
#define WRLVL_DELAY_1_ADDR      (0x830)
#define WRLVL_DELAY_0_ADDR      (0x830)
#define WRLVL_DELAY_8_OFFSET    16
#define WRLVL_DELAY_7_OFFSET    8
#define WRLVL_DELAY_6_OFFSET    0
#define WRLVL_DELAY_5_OFFSET    56
#define WRLVL_DELAY_4_OFFSET    48
#define WRLVL_DELAY_3_OFFSET    40
#define WRLVL_DELAY_2_OFFSET    32
#define WRLVL_DELAY_1_OFFSET    24
#define WRLVL_DELAY_0_OFFSET    16

#define RDLVL_GATE_DELAY_8_ADDR     (0x7f0)
#define RDLVL_GATE_DELAY_7_ADDR     (0x7f0)
#define RDLVL_GATE_DELAY_6_ADDR     (0x7f0)
#define RDLVL_GATE_DELAY_5_ADDR     (0x7f0)
#define RDLVL_GATE_DELAY_4_ADDR     (0x7f0)
#define RDLVL_GATE_DELAY_3_ADDR     (0x7f0)
#define RDLVL_GATE_DELAY_2_ADDR     (0x7e0)
#define RDLVL_GATE_DELAY_1_ADDR     (0x7e0)
#define RDLVL_GATE_DELAY_0_ADDR     (0x7e0)
#define RDLVL_GATE_DELAY_8_OFFSET    40
#define RDLVL_GATE_DELAY_7_OFFSET    32
#define RDLVL_GATE_DELAY_6_OFFSET    24
#define RDLVL_GATE_DELAY_5_OFFSET    16
#define RDLVL_GATE_DELAY_4_OFFSET    8
#define RDLVL_GATE_DELAY_3_OFFSET    0
#define RDLVL_GATE_DELAY_2_OFFSET    56
#define RDLVL_GATE_DELAY_1_OFFSET    48
#define RDLVL_GATE_DELAY_0_OFFSET    40

#define RDLVL_DELAY_8_ADDR      (0x230)
#define RDLVL_DELAY_7_ADDR      (0x230)
#define RDLVL_DELAY_6_ADDR      (0x220)
#define RDLVL_DELAY_5_ADDR      (0x220)
#define RDLVL_DELAY_4_ADDR      (0x210)
#define RDLVL_DELAY_3_ADDR      (0x210)
#define RDLVL_DELAY_2_ADDR      (0x200)
#define RDLVL_DELAY_1_ADDR      (0x200)
#define RDLVL_DELAY_0_ADDR      (0x1f0)
#define RDLVL_DELAY_8_OFFSET    40
#define RDLVL_DELAY_7_OFFSET    8
#define RDLVL_DELAY_6_OFFSET    40
#define RDLVL_DELAY_5_OFFSET    8
#define RDLVL_DELAY_4_OFFSET    40
#define RDLVL_DELAY_3_OFFSET    8
#define RDLVL_DELAY_2_OFFSET    40
#define RDLVL_DELAY_1_OFFSET    8
#define RDLVL_DELAY_0_OFFSET    40
#endif

//------------------------
//define for ddr configure register param location
#define EIGHT_BANK_MODE_ADDR     (0x10)
#define COLUMN_SIZE_ADDR         (0x50)
#define ADDR_PINS_ADDR           (0x50)
#define CS_MAP_ADDR              (0x70)
#define REDUC_ADDR               (0x30)
#define CTRL_RAW_ADDR            (0x40)
#define ECC_DISABLE_W_UC_ERR_ADDR   (0x10)
#define EIGHT_BANK_MODE_OFFSET  32
#define COLUMN_SIZE_OFFSET      24
#define ADDR_PINS_OFFSET        8
#define CS_MAP_OFFSET           16
#define REDUC_OFFSET            8
#define CTRL_RAW_OFFSET         48 
#define ECC_DISABLE_W_UC_ERR_OFFSET 24
//------------------------

//------------------------------------
//for ddr3_level.S
#define SWLVL_RESP_8_ADDR       (0x7b0)
#define SWLVL_RESP_7_ADDR       (0x7b0)
#define SWLVL_RESP_6_ADDR       (0x7a0)
#define SWLVL_RESP_5_ADDR       (0x7a0)
#define SWLVL_RESP_4_ADDR       (0x7a0)
#define SWLVL_RESP_3_ADDR       (0x7a0)
#define SWLVL_RESP_2_ADDR       (0x7a0)
#define SWLVL_RESP_1_ADDR       (0x7a0)
#define SWLVL_RESP_0_ADDR       (0x7a0)

#define SWLVL_START_ADDR        (0x960)
#define SWLVL_START_OFFSET      40
#define SWLVL_LOAD_ADDR         (0x960)
#define SWLVL_LOAD_OFFSET       32
#define SWLVL_EXIT_ADDR         (0x960)
#define SWLVL_EXIT_OFFSET       24
#define SW_LEVELING_MODE_ADDR   (0x750)
#define SW_LEVELING_MODE_OFFSET 48
#define SWLVL_OP_DONE_ADDR      (0x720)
#define SWLVL_OP_DONE_OFFSET    8

#ifdef  loongson3A3
#define WRLVL_EN_ADDR           (0x980)
#define WRLVL_EN_OFFSET         32
#define WRLVL_REG_EN_ADDR       (0x980)
#define WRLVL_REG_EN_OFFSET     40
#define RDLVL_GATE_REG_EN_ADDR  (0x980)
#define RDLVL_GATE_REG_EN_OFFSET 0
#define RDLVL_REG_EN_ADDR       (0x980)
#define RDLVL_REG_EN_OFFSET     8
#else
#define WRLVL_EN_ADDR           (0x710)
#define WRLVL_EN_OFFSET         48
#endif
#define WRLVL_CS_ADDR           (0x750)
#define WRLVL_CS_OFFSET         56
#define RDLVL_GATE_EN_ADDR      (0x720)
#define RDLVL_GATE_EN_OFFSET    40
#define RDLVL_EN_ADDR           (0x720)
#define RDLVL_EN_OFFSET         32
#define RDLVL_CS_ADDR           (0x750)
#define RDLVL_CS_OFFSET         40
#define RDLVL_EDGE_ADDR         (0x940)
#define RDLVL_EDGE_OFFSET       24

.set noreorder

#define	PRINTSTR(x) \
	.rdata;98: .asciz x; .text; dla a0, 98b; bal puts; nop

#######################################################
/* Undefine the two to enable both */
/* !!!NOT USE ANYMORE !!!!! */
///////#define MC0_ONLY
///////#define MC1_ONLY

/* Size of each DDR controller */
/* !!!NOT USE ANYMORE !!!!! */
//#define DDR_512
//#define DDR_1G
//#define DDR_2G


#######################################################

/***********************************************************
| s1:						 |		   |				   |						   |
|[36:36]| FREQ_ADDAPTER      | 1'b1    |				   |	
|       |				     | 1'b0    | Enable freq adapt |	
|       |					 | 1'b1    | Disable freq adapt|  	
|[35:35]| MC1_DIMM_TYPE      | 1'b1    | Registered Dual   |	
|       |                    | 1'b0    | Unbuffered Dual   |
|[34:34]| MC1_ECC            | 1'b1    | WITH DATA ECC     |	
|       |                    | 1'b0    | NO DATA ECC       |
|[33:33]| MC0_DIMM_TYPE      | 1'b1    | Registered Dual   |	
|       |                    | 1'b0    | Unbuffered Dual   |
|[32:32]| MC0_ECC            | 1'b1    | WITH DATA ECC     |	
|       |                    | 1'b0    | NO	  DATA ECC     |
|[31:31]|                    | 1'b0    | NOT USED          |
|[30:28]| MC1_ROW            | MC0_ROW | 15 - MC1_ROW_SIZE |
|[27:27]| MC1_EIGHT_BANK     | 1'b0    | FOUR  BANKS       |
|       |                    | 1'b1    | EIGHT BANKS       |
|[26:24]| MC1_COL            | MC0_COL | 14 - MC1_COL_SIZE |
|[22:20]| MC0_ROW            | MC0_ROW | 15 - MC0_ROW_SIZE |
|[19:19]| MC0_EIGHT_BANK     | 1'b0    | FOUR  BANKS       |
|       |                    | 1'b1    | EIGHT BANKS       |
|[18:16]| MC0_COL_SIZE       | MC0_COL | 14 - COL_SIZE     |
|[15:12]| MC1_CS_MAP         |         |                   |
|[11: 8]| MC0_CS_MAP         |         |                   |
|[ 7: 7]| DDR_TYPE           | 1'b0    | DDR2              |
|       |                    | 1'b1    | DDR3              |
|[ 6: 4]| SIZE_PER_CONTROLLER| 3'b001  | 512MB             |
|       |                    | 3'b010  | 1G                |
|       |                    | 3'b011  | 2G                |
|       |                    | 3'b100  | 4G                |
|[ 3: 2]| CONTROLLER_SELECT  | 2'b00   | USE BOTH          |
|       |                    | 2'b01   | MC0_ONLY          |
|       |                    | 2'b10   | MC1_ONLY          |
|[ 1: 0]| NODE ID            |         |                   |
***********************************************************/
#define GET_NODE_ID_a0  dli a0, 0x00000003; and a0, s1, a0; dsll a0, 44;
#define GET_MC0_ONLY    dli a0, 0x00000004; and a0, s1, a0;
#define GET_MC1_ONLY    dli a0, 0x00000008; and a0, s1, a0;
#define GET_DDR_ADAPTER dli a0, 0x1000000000; and a0, s1, a0;
#define GET_DDR_SIZE    dli a0, 0x00000070; and a0, s1, a0;
#define GET_DDR_TYPE    dli a1, 0x00000080; and a1, s1, a1;
#define GET_MC0_CS_MAP  dli a1, 0x00000f00; and a1, s1, a1; dsll a1, 8
#define GET_MC1_CS_MAP  dli a1, 0x0000f000; and a1, s1, a1; dsll a1, 4;
#define GET_MC0_EIGHT   dli a1, 0x00080000; and a1, s1, a1; dsll a1, 13;
#define GET_MC0_ROW     dli a1, 0x00700000; and a1, s1, a1; dsrl a1, 12;
#define GET_MC0_COL     dli a1, 0x00070000; and a1, s1, a1; dsll a1, 8;
#define GET_MC1_EIGHT   dli a1, 0x08000000; and a1, s1, a1; dsll a1, 5;
#define GET_MC1_ROW     dli a1, 0x70000000; and a1, s1, a1; dsrl a1, 20;
#define GET_MC1_COL     dli a1, 0x07000000; and a1, s1, a1;
#define GET_MC0_ECC     dli a1, 0x100000000; and a1, s1, a1; dsrl a1, 32;
#define GET_MC0_DIMM    dli a1, 0x200000000; and a1, s1, a1; dsrl a1, 33;
#define GET_MC1_ECC     dli a1, 0x400000000; and a1, s1, a1; dsrl a1, 34;
#define GET_MC1_DIMM    dli a1, 0x800000000; and a1, s1, a1; dsrl a1, 35;
#define XBAR_CONFIG_NODE_a0(OFFSET, BASE, MASK, MMAP) \
						daddi   v0, t0, OFFSET;       \
                        dli     t1, BASE;             \
                        or      t1, t1, a0;           \
                        sd      t1, 0x00(v0);         \
                        dli     t1, MASK;             \
                        sd      t1, 0x40(v0);         \
                        dli     t1, MMAP;             \
                        sd      t1, 0x80(v0);
#######################################################
	mymain:
	.global mymain
	move k0,ra
	PRINTSTR  ("\r\nbegin\r\n")

#   	dli     s1, 0x001c03b4  //set use MC1 or MC0 or MC1/0 and give All DIMM infor
   	dli     s1, 0x1c0030b8  //set use MC1 or MC0 or MC1/0 and give All DIMM infor
#   	dli     s1, 0x1c1c33b8  //set use MC1 or MC0 or MC1/0 and give All DIMM infor

###Set DDR frequency ### 
#if 1 
		/* software ddr clk_sel can take effect when hardware ddrclk is in bypass mode */
        dli     t2, 0x900000001fe00180
        lw  a1, 0x14(t2)
        li  a0, 0x000003e0
        and a1, a1, a0
        bne a0, a1, no_software_ddrclksel
        nop 

        PRINTSTR  ("\r\nSet DDR frequency by software\r\n")
        dli t2, 0x900000001fe00180

		/* clear ddr_clksel */
        lw  a1, 0x0(t2)
        li  a0, 0xe0ffffff
        and a1, a1,a0

		/* set ddr_clksel according memory size and scatter in DIMMS */
		// NOTICE HERE: autocalily decrease ddr freq only when two ddr in one channel 
		// if decrease freq     : ddr_clksel = 5'10110 means X09
		// if not decrease freq : ddr_clksel = 5'00110 means X12

		GET_DDR_ADAPTER 
		beqz  a0, 1f
		nop
        li  a0, 0x16000000 // ddr_freq X 0x09
		b	2f
		nop
1:
        li  a0, 0x06000000 // ddr_freq X 0x12
2:
        or  a1, a1, a0
        sw  a1, 0x0(t2)

		/* enable software ddr_clksel */
        li  a0, 0x00000008
        or  a1, a1, a0
        sw  a1, 0x0(t2)

no_software_ddrclksel:

#endif
        

        GET_MC0_CS_MAP
        beqz    a1, 1f
        nop

		dli     t3, 0x0
	    bal	    mc_init
	    nop

	    PRINTSTR("\r\nMC0 Config DONE\r\n")
1:

        GET_MC1_CS_MAP
        beqz    a1, 1f
        nop


		dli     t3, 0x1
	    bal	    mc_init
	    nop

1:
	    PRINTSTR("\r\nMC1 Config DONE\r\n")

/* MC0_ONLY */
        GET_MC0_ONLY
        beqz    a0, 1f
        nop
	    PRINTSTR("MC0 space open : 0x00000000 - 0x0FFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x0, \
                            0x0000000000000000, \
                            0xfffffffff0000000, \
                            0x00000000000000f0)

        b       2f
        nop
1:
/* MC1_ONLY */
        GET_MC1_ONLY
        beqz    a0, 1f
        nop
	    PRINTSTR("MC1 space open : 0x00000000 - 0x0FFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x0, \
                            0x0000000000000000, \
                            0xfffffffff0000000, \
                            0x00000000000000f1)

        b       2f
        nop
1:
	    PRINTSTR("DDR space open : 0x00000000 - 0x0FFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x0, \
                            0x0000000000000000, \
                            0xfffffffff0000000, \
                            0x00000000000000f0)


2:
/* MC0_ONLY */
        GET_MC0_ONLY
        beqz    a0, 1f
        nop

        GET_DDR_SIZE
        dli     a1, 0x10
        beq     a1, a0, ddr_512MB_MC0
        nop
        dli     a1, 0x20
        beq     a1, a0, ddr_1GB_MC0
        nop
        dli     a1, 0x30
        beq     a1, a0, ddr_2GB_MC0
        nop

ddr_4GB_MC0:
	    PRINTSTR("MC0 space open : 0x100000000 - 0x1FFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000100000000, \
                            0xffffffff00000000, \
                            0x00000000000000f0)
        b       2f
        nop

ddr_2GB_MC0:
	    PRINTSTR("MC0 space open : 0x80000000 - 0xFFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000080000000, \
                            0xffffffff80000000, \
                            0x00000000000000f0)
        b       2f
        nop
ddr_1GB_MC0:
	    PRINTSTR("MC0 space open : 0x80000000 - 0xbFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000080000000, \
                            0xffffffffc0000000, \
                            0x00000000000000f0)
        b       2f
        nop
ddr_512MB_MC0:
	    PRINTSTR("MC0 pace open : 0x20000000 - 0x2FFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000020000000, \
                            0xfffffffff0000000, \
                            0x00000000100000f0)
        b       2f
        nop

1:
/* MC1_ONLY */
        GET_MC1_ONLY
        beqz    a0, 1f
        nop

        GET_DDR_SIZE
        dli     a1, 0x10
        beq     a1, a0, ddr_512MB_MC1
        nop
        dli     a1, 0x20
        beq     a1, a0, ddr_1GB_MC1
        nop
        dli     a1, 0x30
        beq     a1, a0, ddr_2GB_MC1
        nop

ddr_4GB_MC1:
	    PRINTSTR("MC1 space open : 0x100000000 - 0x1FFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000100000000, \
                            0xffffffff00000000, \
                            0x00000000000000f1)
        b       2f
        nop


ddr_2GB_MC1:
	    PRINTSTR("MC1 space open : 0x80000000 - 0xFFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000080000000, \
                            0xffffffff80000000, \
                            0x00000000000000f1)
        b       3f
        nop
ddr_1GB_MC1:
	    PRINTSTR("MC1 space open : 0x80000000 - 0xbFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000080000000, \
                            0xffffffffc0000000, \
                            0x00000000000000f1)
        b       3f
        nop
ddr_512MB_MC1:
	    PRINTSTR("MC1 space open : 0x20000000 - 0x2FFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000020000000, \
                            0xfffffffff0000000, \
                            0x00000000100000f1)
3:
	    PRINTSTR("PCI DMA SPACE reassigned to MC1\r\n")
        dli     t1, 0x00000000000000f1
        sd      t1, 0x180(t0)

        b       2f
        nop

1:
#define PER_CONTROLLER_512M 0x10
#define PER_CONTROLLER_1G	0x20
#define PER_CONTROLLER_2G	0x30
#define PER_CONTROLLER_4G	0x40

		GET_DDR_SIZE    
//		beq	a0, PER_CONTROLLER_512M, interleave_512m  ; almost no 512M mem used now
		nop
		beq	a0, PER_CONTROLLER_1G, interleave_1g 
		nop
		beq	a0, PER_CONTROLLER_2G, interleave_2g 
		nop
		beq	a0, PER_CONTROLLER_4G, interleave_4g 
		nop
		

interleave_1g:
	    PRINTSTR("DDR space open : 0x80000000 - 0xFFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000080000000, \
                            0xffffffffc0000000, \
                            0x00000000000000f0)
        XBAR_CONFIG_NODE_a0(0x38, \
                            0x00000000c0000000, \
                            0xffffffffc0000000, \
                            0x00000000000000f1)

	    PRINTSTR("PCI DDR space open : 0x80000000 - 0xFFFFFFFF\r\n")

        // Disable the default Windows
        dli     t1, 0x0
        sd      t1, 0x180(t0)

        // Enable others
        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x120, \
                            0x0000000080000000, \
                            0xffffffffc0000000, \
                            0x00000000000000f0)
        XBAR_CONFIG_NODE_a0(0x138, \
                            0x00000000c0000000, \
                            0xffffffffc0000000, \
                            0x00000000000000f1)

		  b  interleave_out;
		  nop

interleave_2g:
	    PRINTSTR("DDR space open : 0x100000000 - 0x1FFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000100000000, \
                            0xffffffff80000000, \
                            0x00000000000000f0)
        XBAR_CONFIG_NODE_a0(0x38, \
                            0x0000000180000000, \
                            0xffffffff80000000, \
                            0x00000000000000f1)

	    PRINTSTR("PCI DDR space open : 0x100000000 - 0x1FFFFFFFF\r\n")

        // Disable the default Windows
        dli     t1, 0x0
        sd      t1, 0x180(t0)

        // Enable others
        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x120, \
                            0x0000000100000000, \
                            0xffffffff80000000, \
                            0x00000000000000f0)
        XBAR_CONFIG_NODE_a0(0x138, \
                            0x0000000180000000, \
                            0xffffffff80000000, \
                            0x00000000000000f1)

		  b  interleave_out;
		  nop

interleave_4g:
	    PRINTSTR("DDR space open : 0x200000000 - 0x3FFFFFFFF\r\n")

        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x20, \
                            0x0000000200000000, \
                            0xffffffff00000000, \
                            0x00000000000000f0)
        XBAR_CONFIG_NODE_a0(0x38, \
                            0x0000000300000000, \
                            0xffffffff00000000, \
                            0x00000000000000f1)

	    PRINTSTR("PCI DDR space open : 0x200000000 - 0x3FFFFFFFF\r\n")

        // Disable the default Windows
        dli     t1, 0x0
        sd      t1, 0x180(t0)

        // Enable others
        GET_NODE_ID_a0;
        XBAR_CONFIG_NODE_a0(0x120, \
                            0x0000000200000000, \
                            0xffffffff00000000, \
                            0x00000000000000f0)
        XBAR_CONFIG_NODE_a0(0x138, \
                            0x0000000300000000, \
                            0xffffffff00000000, \
                            0x00000000000000f1)



2:

interleave_out:


        
PRINTSTR("done\r\n");
move ra,k0
jr ra
nop



puts:
	dli v0,SERIAL+0xffffffffff200000
1:
	lbu	v1, 0(a0)
	beqz	v1, 2f
	daddiu	a0, 1
	b	1b
	sb v1,(v0)
2:
	j	ra
	nop



/**********************************
    Author: chenxinke
    Date:   20110125
    v2.0
        support both 3A and 3A3 according macro defination--loongson3A3
        MC0/1 both use the same subroutine, add an input param: t3.
    input param:
        t8: input, Memory Controller config register base
        t3: controller select(config param select)
            0: MC0
            1: MC1
**********************************/
//#define MULTI_NODE_DDR_PARAM
#define CONFIG_BASE 0x900000000ff00000

        .global ddr2_config
        .ent    ddr2_config
        .set    noreorder
        .set    mips3
ddr2_config:
    dla     a2, ddr2_reg_data

    dli     t1, DDR_PARAM_NUM
    daddiu  v0, t8, 0x0
//write param registers
1:
    ld      a1, 0x0(a2)
    sd      a1, 0x0(v0)
    daddiu  t1, t1, -1
    daddiu  a2, a2, 0x8
    daddiu  v0, v0, 0x10
    bnez    t1, 1b
    nop
#if 1
    //rewrite eight_bank_mode
    ld      a2, EIGHT_BANK_MODE_ADDR(t8)
    dli     a1, 0xff
    dsll    a1, a1, EIGHT_BANK_MODE_OFFSET
    not     a1, a1
    and     a2, a2, a1
	GET_MC0_EIGHT
	beqz t3,1f
	nop
	GET_MC1_EIGHT
1:
    or      a2, a2, a1
    sd      a2, EIGHT_BANK_MODE_ADDR(t8)
    //rewrite column_size and addr_pins
    ld      a2, COLUMN_SIZE_ADDR(t8)
    dli     a1, 0xffffffff00ff00ff
    and     a2, a2, a1
	GET_MC0_ROW
	beqz t3,1f
	nop
	GET_MC1_ROW
1:
    or      a2, a2, a1
	GET_MC0_COL
	beqz t3,1f
	nop
	GET_MC1_COL
1:
    or      a2, a2, a1
    sd      a2, COLUMN_SIZE_ADDR(t8)
    //rewrite cs_map
    ld      a2, CS_MAP_ADDR(t8)
    dli     a1, 0xff
    dsll    a1, a1, CS_MAP_OFFSET
    not     a1, a1
    and     a2, a2, a1

	GET_MC0_CS_MAP
	beqz t3,1f
	nop
	GET_MC1_CS_MAP
1:
    or      a2, a2, a1
    sd      a2, CS_MAP_ADDR(t8)
    //set data bus width
    ld      a2, REDUC_ADDR(t8)
    dli     a1, 0xff
    dsll    a1, a1, REDUC_OFFSET
    not     a1, a1
    and     a2, a2, a1
#if 0
    GET_DIMM_WIDTH
    dsll    a1, a1, REDUC_OFFSET
    or      a2, a2, a1
    sd      a2, REDUC_ADDR(t8)
#endif
    //disable ECC module here for ARB_level, ECC will be enabled later
    ld      a2, CTRL_RAW_ADDR(t8)
    dli     a1, 0xff
    dsll    a1, a1, CTRL_RAW_OFFSET
    not     a1, a1
    and     a2, a2, a1
    dli     a1, 0x02
    dsll    a1, a1, CTRL_RAW_OFFSET
    or      a2, a2, a1
    sd      a2, CTRL_RAW_ADDR(t8)
#endif



    sync
    ############start##########
    /***** set start to 1,start to initialize SDRAM *****/
    daddiu    v0, t8, 0x30
    dli     a2, 0x0000010000000000
    ld      a1, 0x0(v0)
    or      a1, a1, a2
    sd      a1, 0x0(v0)
    sync

    //wait initialization complete 
    daddiu  v0, t8, 0x960
1:
    ld      a1, 0x0(v0)
    andi    a1, a1, 0x100
    beqz    a1, 1b
    nop

    //wait(or make sure) dll is locked
    daddiu  v0, t8, 0x10
1:
    ld      a1, 0x0(v0)
    andi    a1, a1, 0x1
    beqz    a1, 1b
    nop

#if 0
//too long  delay can cause problem(stuck).
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!
    dli     a2, 0x4000
1:
    daddiu  a2, a2, -1
    bnez    a2, 1b
    nop
#endif

    ###############################
ddr2_config_end:
    jr      ra
    nop
    .end    ddr2_config


//subroutine: mc_init
//input: t3--MC select, 0--MC0; 1--MC1
        .global mc_init
        .ent    mc_init
        .set    noreorder
        .set    mips3
mc_init:
    move    t4, ra

    GET_NODE_ID_a0
    dli     t2, 0x900000001fe00180
    dli     t0, 0x900000003ff00000
    or      t2, t2, a0
    or      t0, t0, a0

#if 1 // AdonWang disable cpu buffered read
/* !!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!! */
    PRINTSTR("Disable cpu buffered read\r\n")
    lw      a1, 0x0(t2)
    li      a0, 0xfffffdff
    and     a1, a1, a0
    sw      a1, 0x0(t2)
#endif

#if 1 // AdonWang disable ddr3 readbuff
/*      May Affect the Performance     */
//This seems better for the spec2000
//if we enable ECC, this bit will be reset in mc_init
    PRINTSTR("Disable read buffer\r\n")
    lw      a1, 0x4(t2)
    li      a0, 0x18
    or      a1, a1, a0
    sw      a1, 0x4(t2)
#endif
    PRINTSTR("\r\nEnable register space of MEMORY\r\n")
    lw      a1, 0x0(t2)
    li      a0, 0xfffffeff
    and     a1, a1,a0
    sw      a1, 0x0(t2)

    GET_NODE_ID_a0;

    bnez    t3, 1f
    nop

    XBAR_CONFIG_NODE_a0(0x0, \
                    0x0000000000000000, \
                    0xfffffffff0000000, \
                    0x00000000000000f0)
	b 2f
    nop
1:
    XBAR_CONFIG_NODE_a0(0x0, \
                    0x0000000000000000, \
                    0xfffffffff0000000, \
                    0x00000000000000f1)

2:
    sync

    dli     a1, 0x900000000ff00000
    or      t8, a0, a1
    bal     ddr2_config
    nop
	PRINTSTR("ddr2_config done\r\n");
#if 0   //def  DEBUG_DDR_PARAM   //print registers
    PRINTSTR("The MC param is:\r\n")
    dli     t1, DDR_PARAM_NUM
    GET_NODE_ID_a0
    dli     t5, 0x900000000ff00000
    or      t5, t5, a0
1:
    ld      t6, 0x0(t5)
    dsrl    a0, t6, 32
    bal     hexserial
    nop
    PRINTSTR("  ")
    move    a0, t6
    bal     hexserial
    nop
    PRINTSTR("\r\n")

    daddiu  t1, t1, -1
    daddiu  t5, t5, 16
    bnez    t1, 1b
    nop
#endif

    PRINTSTR("Disable register space of MEMORY\r\n")
    lw      a1,0x0(t2)
    or      a1,a1,0x100
    sw      a1,0x0(t2)

//cxk

    move    ra, t4
    jr      ra
    nop
    .end    mc_init


    .text
    .global ddr2_reg_data

	.align 3
ddr2_reg_data:
.dword 0x0000010000000100
.dword 0x0001000100010001
.dword 0x0100010000000000
.dword 0x0101000001000000
.dword 0x0002010200000101
.dword 0x0200000404060100
.dword 0x0a05040804040003
.dword 0x0f0e000000010c0c
.dword 0x0804020100000000
.dword 0x0000091100000000
.dword 0x0000003f3f1b0418
.dword 0x0000000000000000
.dword 0x0000560814000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000900
.dword 0xffff000000000000
.dword 0x52100003020000c8
.dword 0x0000204002000060
.dword 0x00000000000340d0
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000007
.dword 0x0000000000000000
.dword 0x0204070400000001
.dword 0x0000000000000206
.dword 0x001e008000000000
.dword 0x001e0080001e0080
.dword 0x001e008000200080
.dword 0x001e0080001e0080
.dword 0x001e0080001e0080
.dword 0x0000200000002000
.dword 0x0000220000002000
.dword 0x0000200000002000
.dword 0x0000200000002000
.dword 0x0000000000002000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0xf400373303c009bd
.dword 0xf4003733f4003733
.dword 0xf4003733f4003733
.dword 0xf4003733f4003733
.dword 0xf4003733f4003733
.dword 0x26c0000126c00001
.dword 0x26c0000126c00001
.dword 0x26c0000126c00001
.dword 0x26c0000126c00001
.dword 0x0800e10026c00001
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0100000000000000
.dword 0x0000000101020101
.dword 0x0303030a00030002
.dword 0x0101010202020203
.dword 0x0102020400060c01
.dword 0x2819000000000f0f
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0xee0000ee00400000
.dword 0x0000000000000c00
.dword 0x0000640064000000
.dword 0x0000000000000064
.dword 0x0000004000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x00000000001c001c
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000041104000000
.dword 0x0000000030000000
.dword 0x0000000010101080
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0xff04040000060600
.dword 0x0000000000000d00
.dword 0x0705000000040100
.dword 0x000000000003e825
.dword 0x0001010001000101
.dword 0x0404040404040400
.dword 0x0707050500090004
.dword 0x02000100000a000f
.dword 0x04200c2d0c2d0c2d
.dword 0x0044042004200420
.dword 0x0000004400440044
.dword 0x0000000000000000
.dword 0x007f000000000000
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x001a001800160000
.dword 0x001c001c001c001a
.dword 0x00000000001c001c
.dword 0x0000000000000000
.dword 0x0004000000000000
.dword 0x000a000800080006
.dword 0x000a000800080006
.dword 0x0016ffff00000010
.dword 0x001c001a001a0018
.dword 0x001c001c001c001c
.dword 0x0000000000000000
.dword 0x0000000000000000
.dword 0x0012001200000000
.dword 0x0030001c00140012
.dword 0x0000001c00360030
.dword 0x00000c2d00000c2d
.dword 0x00000c2d00000c2d



