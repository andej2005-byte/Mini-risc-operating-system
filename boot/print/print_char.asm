print_char:
	pusha
	
	mov ah, 0x0e
	mov bh, 0x00
	
	int 0x10
	
	popa
	ret
