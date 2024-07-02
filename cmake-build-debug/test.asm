section .data
	var2_1: dq 3

	var_str0: db '2', 0
	var_str1: db '3', 0
	var_str2: db '1', 0
	var_str3: db '14.000000', 0
section .text
	 global _start
_start:
	push rbp
	mov rbp, rsp
	sub rsp, 1000h

	mov r10, 1
	mov r11, 1
	cmp r10, r11
	je L2
	mov r10, 0
	jmp L3
L2:
	mov r10, 1
L3:
L3:
	cmp r10, 0
	je L0
	mov eax, 4
	mov ebx, 1
	mov ecx, var_str2
	mov edx, 2
	int 80h

	jmp L1
L0:
L1:
	mov r10, 6
	mov r11, 2
	add r10, r11
	push rax
	call func1

	add rsp, 8
	mov eax, 4
	mov ebx, 1
	mov ecx, var_str3
	mov edx, 2
	int 80h


	mov rsp, rbp
	pop rbp
	mov rax, 0
	int 80h

	mov eax,1
	mov ebx,0
	int 80h

func1:
	push rbp
	mov rbp, rsp


	mov eax, 4
	mov ebx, 1
	mov ecx, var_str0
	mov edx, 2
	int 80h

	mov qword [var2_1], 3

	mov eax, 4
	mov ebx, 1
	mov ecx, var_str1
	mov edx, 2
	int 80h

	pop rbp
	ret
