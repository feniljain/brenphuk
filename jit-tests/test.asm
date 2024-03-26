section .data
    a db 10

section .text
global _start

_start:
	mov rdi, a
	mov rsi, a

; rdi -> tape address
; rsi -> pointer address

loop_start:
    cmp rdi, 0
    jz exit

    cmp rdi, 0
    jnz loop_start
exit:
