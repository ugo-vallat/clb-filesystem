.global get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.global put32
put32:
	str w1,[x0]
	ret

.global get32
get32:
	ldr w0,[x0]
	ret

.global delay
delay:
	subs x0, x0, #1
	bne delay
	ret

.global set_pgd
set_pgd:
	msr	ttbr0_el1, x0
	tlbi vmalle1is
  	DSB ISH              // ensure completion of TLB invalidation
	isb
	ret

.global get_pgd
get_pgd:
	mov x1, 0
	ldr x0, [x1]
	mov x0, 0x1000
	msr	ttbr0_el1, x0
	ldr x0, [x1]
	ret

