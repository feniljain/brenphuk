loop_start:
	mov rcx, [rdi]
	cmp rcx, 0
	jnz continue
	ret
continue:

; INCREMENT
mov rcx, [rdi]
add rcx, rsi
mov [rdi], rcx

; FWD
add rdi, 1

; DECREMENT
mov rcx, [rdi]
sub rcx, rsi
mov [rdi], rcx

; BWD
sub rdi, 1

cmp rdi, 0
jnz loop_start 
