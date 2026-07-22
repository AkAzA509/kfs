global update_gdt

section .text
update_gdt:
	mov eax, [esp + 4]
	lgdt [eax]

	mov ax, 0x10		; Kernel data segment offset (idx 2 in the gdt table)
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ax, 0x18		; Kernel stack segment offset (idx 3 in the gdt table)
	mov ss, ax

	jmp 0x08:done		; 0x08 tell to asm that a changement of segment occur (far jump needed)
						; 0x08 Kernel code segment offset (idx 1 in the gdt table)
done:
	ret
