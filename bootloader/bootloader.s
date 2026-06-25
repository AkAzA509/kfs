global main

section .data
    msg db "hello world", 10, 0

section .text
main:
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel msg] 
    mov rdx, 13
    syscall
    cmp rax, 0
    jl .exit
    ret

.exit:
    mov rdi, rax
    mov rax, 60
    syscall
    cmp rax, 0
    jl .exit
    ret    