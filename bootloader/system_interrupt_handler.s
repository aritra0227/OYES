; This is the initial layer of the interrupt_handler from where the interrupt handlers are triggered
; This module takes of calling the respective interrupt handlers through the "interrupt_handler" C function while maintaining the initial state of the registers

extern interrupt_handler

%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword 0    ; assigning error code 0 for interrupts which do not produce error codes
    push dword %1   ; push interrupt number
    jmp common_interrupt_handler 
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword %1   ; push interrupt number
    jmp common_interrupt_handler
%endmacro

common_interrupt_handler:
    push esp
    add dword [esp], 8 ;this line is needed so that the stack pointer points at the stack state when calling iret
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi
    push edi
    call interrupt_handler
    pop edi; 
    pop esi;
    pop ebp;
    pop edx;
    pop ecx;
    pop ebx;
    pop eax;
    pop esp
    iret

;some software interrupts
no_error_code_interrupt_handler 0   ;expands as interrupt_handler_0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler 8      ;expands as interrupt_handler_8
no_error_code_interrupt_handler 9
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20


;hardware/PIC interrupts (remmapped starting at 0x20 = 32)
no_error_code_interrupt_handler 32
no_error_code_interrupt_handler 33
no_error_code_interrupt_handler 34
no_error_code_interrupt_handler 35
no_error_code_interrupt_handler 36
no_error_code_interrupt_handler 37
no_error_code_interrupt_handler 38
no_error_code_interrupt_handler 39
no_error_code_interrupt_handler 40
no_error_code_interrupt_handler 41
no_error_code_interrupt_handler 42
no_error_code_interrupt_handler 43
no_error_code_interrupt_handler 44
no_error_code_interrupt_handler 45
no_error_code_interrupt_handler 46
no_error_code_interrupt_handler 47



global enable_interrupts
enable_interrupts:
    sti
    ret