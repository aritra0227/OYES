global user_entry

user_entry:
    mov eax, 0xc00b8000
    mov byte[eax], 'U'
    mov eax, 0xc00b8001
    mov byte[eax], 0x10
    jmp $