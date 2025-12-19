
.global memwrite
memwrite:
    mov x4, x2
b_memewrite:
	ldrb w3, [x0], #1
	strb w3, [x1], #1
	subs x4, x4, #1
	b.gt b_memewrite
    sub x0,x2,x4
	ret