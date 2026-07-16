global update_gdt

section .text
update_gdt:
	lgdt [rdi]
	jmp 0x08:reload_cs

reload_cs:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax