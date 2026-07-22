global init_fpu

section .text
init_fpu:

	; Initialize the FPU: enable hardware floating point instructions
	; and reset it to a known clean state before any C code can run.

	mov eax, cr0
	and eax, ~(1 << 2)		; clear EM: allow FPU instructions (no emulation)
	or  eax, (1 << 1)		; set MP: standard WAIT/FWAIT behavior
	or  eax, (1 << 5)		; set NE: use modern #MF exceptions
	mov cr0, eax
	fninit					; reset FPU to a clean known state
	ret