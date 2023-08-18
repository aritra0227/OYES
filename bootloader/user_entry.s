global user_entry

user_entry:
    mov eax, 0xc00b8000
    mov word[eax], 'U'
    mov eax, 0xc00b8001
    mov word[eax], 0x9
    jmp $