.set mips64
_start:
dli a0,0x9000000010000000
lw v0,(a0)
jr ra
nop
