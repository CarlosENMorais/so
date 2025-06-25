.section .data
mensagem:
    .ascii "Olá, Mundo!\n"
len = . - mensagem

.section .text
.global _start
_start:
    mov $1, %rax
    mov $1, %rdi
    lea mensagem(%rip), %rsi
    mov $len, %rdx
    syscall

    mov $60, %rax
    syscall
