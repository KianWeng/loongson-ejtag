unsigned int aui_singlestepset_code[]=
tgt_compile(\
"1:\n" \
"	dmtc0	$15, $31 ;\n" \
"	dmfc0	$15,  $23;\n" \
"	# Set the SSt bit in the debug register\n" \
"	ori	$15,  0x0100;\n" \
"	# Put the modified debug register back\n" \
"	dmtc0	$15, $23;\n" \
"	dmfc0	$15, $31 ;\n" \
"   sync;\n" \
"	b 1b;\n" \
"	nop;\n" \
);
