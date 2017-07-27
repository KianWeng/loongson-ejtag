static unsigned int aui_singlestepclear_code[]=
tgt_compile(\
"1:\n" \
"	dmtc0	$15, $31 ;\n" \
"	lui	$15, 0xFF20;\n" \
"	sd	$1,  STACK($15);\n" \
"	sd	$2,  STACK($15);\n" \
"	# Load R1 with the debug register (CP0 reg 23 select 0);\n" \
"	dmfc0	$1,  $23;\n" \
"	# Load the AND mask into R2;\n" \
"	lui	$2, 0xFFFF;\n" \
"	ori	$2, $2, 0xFEFF;\n" \
"	# Clear the SSt bit in the debug register;\n" \
"	and	$1, $1, $2;\n" \
"	# Put the modified debug register back;\n" \
"	dmtc0	$1, $23;\n" \
"	# Pop R1 and R2 off the debug stack;\n" \
"	ld	$2,  STACK($15);\n" \
"	ld	$1,  STACK($15);\n" \
"	dmfc0	$15, $31 ;\n" \
"   sync;\n" \
"	b 1b;\n" \
"	nop;\n" \
);
