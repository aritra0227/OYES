global flush_tss
flush_tss:
	mov ax, (5 * 8)
	ltr ax
	ret