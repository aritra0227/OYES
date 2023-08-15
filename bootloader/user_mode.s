global enter_user_mode


USER_MODE_CODE_SEGMENT equ (0x18 | 0x3)
USER_MODE_DATA_SEGMENT equ (0x20| 0x3)
USER_MODE_STACK_POINTER equ 0xC0401000

enter_user_mode:
    push USER_MODE_DATA_SEGMENT
    push USER_MODE_STACK_POINTER
    pushf
    push USER_MODE_CODE_SEGMENT
    push 0x00000000
    iret
