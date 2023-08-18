[bits 32]

global enter_user_mode
extern user_entry


USER_MODE_CODE_SEGMENT equ 0x18 | 0x3
USER_MODE_DATA_SEGMENT equ 0x20| 0x3

section .text
align 4


enter_user_mode:
    ; cli
    ; mov     ss, USER_MODE_DATA_SEGMENT
    mov     eax, USER_MODE_DATA_SEGMENT
    mov     ds, eax
    mov     gs, eax
    mov     es, eax
    mov     fs, eax

    mov eax, esp
    push USER_MODE_DATA_SEGMENT 
    push eax
    pushf
    ; mov cs, USER_MODE_CODE_SEGMENT
    push USER_MODE_CODE_SEGMENT
    ; xchg bx, bx
    mov ebx, user_entry
    ; xchg bx, bx
    push ebx

    ; xchg bx, bx
    iret

