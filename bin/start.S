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
.word	 0
1:
lw	$28, 0($31); //gp
addiu $15,$31,-12;
addu $28,$15;  //fixup gp
lw $12,4($31);
beqz $12,25f;
nop
lw $13,8($31);
addu $13,$15;
addu $13,8;  /*skip first 2 entry*/
addiu $12,-2;
2:lw $2,($13);
addu $2,$15;
addiu $12,-1;
sw $2,($13);
bnez $12,2b;
addu $13,4;
sw  $0, 4($31)
la $12,uboot_end_data;
la $13,uboot_end;
3:
sw $0,($12);
bne $12,$13,3b;
addu $12,4;
25:
move $31, $14
la $25,mymain;
jr $25;
nop;
#elif defined(FIXUP)
move $14,$31;
bal	1f;
nop;
.word  0
1:
sw $14,($31);

la $12,uboot_end_data;
la $13,uboot_end;
3:
sw $0,($12);
bne $12,$13,3b;
addu $12,4;

bal mymain
nop
la $12,1b;
lw $31,-4($12)
.set push
.set nofix_ls_jump1
.set nofix_ls_jump
jr $31
nop
.set pop
#else
la $12,uboot_end_data;
la $13,uboot_end;
3:
sw $0,($12);
bne $12,$13,3b;
addu $12,4;
b mymain
nop
#endif
