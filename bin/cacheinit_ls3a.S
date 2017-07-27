#define CP0_TAGLO $28
#define CP0_TAGHI $29
#define CP0_ECC $26
#define CP0_STATUS $12

#define Index_Store_Tag_I			0x08
#define Index_Store_Tag_D			0x09
#define Index_Invalidate_I			0x00
#define Index_Writeback_Inv_D			0x01
#define Index_Store_Tag_S			0x0b
#define Index_Writeback_Inv_S			0x03
.set mips64
.set noreorder
_start:
.global _start;
	move s0,ra

	mfc0    t0, CP0_STATUS
	li      t1, 0x00e0      # {cu3,cu2,cu1,cu0}<={0110, status_fr<=1
	or      t0, t0, t1
	mtc0    t0, CP0_STATUS

	dli a0, 0x980000001fc00000
	li  a2, 0x00100000   #cache init 1M/4way
        bal scache_init_64_fast
        nop

	bal godson2_cache_init
	nop


	li v0,0x12345678
	move ra,s0
	jr ra
	nop

godson2_cache_init:
	lui     a0, 0x9fc0
	li      a2, (1<<14) #64k/4way

	mtc0    $0, CP0_TAGHI
	mtc0    $0, CP0_TAGLO
	li      a1, 0x22
	daddu    v0, $0, a0
	daddu    v1, a0, a2
1:
	slt     a3, v0, v1
	beq     a3, $0, 2f
	nop
	mtc0    a1, CP0_ECC
	cache   Index_Store_Tag_D, 0x0(v0)
	cache   Index_Store_Tag_D, 0x1(v0)
	cache   Index_Store_Tag_D, 0x2(v0)
	cache   Index_Store_Tag_D, 0x3(v0)

	mtc0    zero, CP0_ECC

	cache   Index_Store_Tag_I, 0x0(v0)
	cache   Index_Store_Tag_I, 0x1(v0)
	cache   Index_Store_Tag_I, 0x2(v0)
	cache   Index_Store_Tag_I, 0x3(v0)
	b	1b
	daddiu   v0, v0, 0x20
2:
	jr      ra
	nop

scache_init_64_fast:

	li      a3, 0x22
	mtc0    a3, CP0_ECC
	mtc0    zero, CP0_TAGHI
	mtc0    zero, CP0_TAGLO
	daddu    v0, zero, a0
	daddu    v1, a0, a2
1:      
	daddiu   v0, v0, 0x80
	 /* body 0*/ 
	cache   Index_Store_Tag_S, -0x80(v0)
	cache   Index_Store_Tag_S, -0x7f(v0)
	cache   Index_Store_Tag_S, -0x7e(v0)
	cache   Index_Store_Tag_S, -0x7d(v0)

	 /* body 1*/
	cache   Index_Store_Tag_S, -0x60(v0)
	cache   Index_Store_Tag_S, -0x5f(v0)
	cache   Index_Store_Tag_S, -0x5e(v0)
	cache   Index_Store_Tag_S, -0x5d(v0)

	 /* body 2*/
	cache   Index_Store_Tag_S, -0x40(v0)
	cache   Index_Store_Tag_S, -0x3f(v0)
	cache   Index_Store_Tag_S, -0x3e(v0)
	cache   Index_Store_Tag_S, -0x3d(v0)

	 /* body 3*/
	cache   Index_Store_Tag_S, -0x20(v0)
	cache   Index_Store_Tag_S, -0x1f(v0)
	cache   Index_Store_Tag_S, -0x1e(v0)
	bne     v0, v1, 1b
	cache   Index_Store_Tag_S, -0x1d(v0)


	jr      ra
	nop

