global load_gdt

load_gdt:
    lgdt [esp + 4] ;load gdt
    ret

global load_segment_registers

; the following code basically refers to the current descriptors we will start with
; the first descriptor (index 1) will be the code segment decsriptor and cs will contain its offset
; the second descriptor (index 2) will be the data segment descriptor and ds,ss,es,fs and gs will contain the offset

load_segment_registers:
    mov eax, 0x10
    mov ds, eax
    mov ss, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    jmp 0x08:flush_cs ; "far jump" to load cs

flush_cs:
    ret
