#include <../include/regdef.h>
.global mymain
mymain:

	lui 	a0, 0x8000
	li	a3, 0			# First TLB index.

	li	a2, 0 # PG_SIZE_4K
	mtc0   a2, c0_pagemask   # Whatever...

1:
	mtc0   a0, c0_entryhi	# Clear entry high.
	mtc0   zero, c0_entrylo0	# Clear entry low0.
	mtc0   zero, c0_entrylo1	# Clear entry low1.

	mtc0    a3, c0_index	# Set the index.
	addiu	a3, 1
	li	a2, 64
	nop
	nop
	tlbwi				# Write the TLB

	bne	a3, a2, 1b
	nop

	jr	ra
	nop
