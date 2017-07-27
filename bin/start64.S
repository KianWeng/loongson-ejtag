#include "../include/regdef.h"
.set noreorder;
.global _start;
_start:
#ifdef __PIC__
move $14,$31;
bal	1f;
nop;
.word	_gp;
.word	num_got_entries 
.word	_GLOBAL_OFFSET_TABLE_ 
.dword  0
1:
lw	$28, 0($31); //gp
daddiu $15,$31,-12;
daddu $28,$15;  //fixup gp
lw $12,4($31);
beqz $12,25f;
nop
lw $13,8($31);
daddu $13,$15;
daddu $13,16; /*skip first 2 entry*/ 
daddiu $12,-2;
2:ld $2,($13);
daddu $2,$15;
daddiu $12,-1;
sd $2,($13);
bnez $12,2b;
daddu $13,8;
sw  $0, 4($31)
dla $12,uboot_end_data;
dla $13,uboot_end;
3:
sw $0,($12);
bne $12,$13,3b;
daddu $12,4;
25:
move $31, $14
dla $25,mymain;
jr $25;
nop;
#elif defined(FIXUP)
/*here nop for alignment*/
nop
move $14,$31;
bal	1f;
nop;
.dword  0
1:
sd $14,($31);

dla $12,uboot_end_data;
dla $13,uboot_end;
3:
sw $0,($12);
bne $12,$13,3b;
daddu $12,4;

bal mymain
nop
dla $12,1b;
ld $31,-8($12)
.set push
.set nofix_ls_jump1
.set nofix_ls_jump
jr $31
nop
.set pop
#else
dla $12,uboot_end_data;
dla $13,uboot_end;
3:
sw $0,($12);
bne $12,$13,3b;
daddu $12,4;
b mymain
nop
#endif
