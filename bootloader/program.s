; set eax to some distinguishable number, to read from the log afterwards
bits 32
start:
    mov eax, 0xBEEFDEAD

; enter infinite loop, nothing more to do
; $ means "beginning of line", ie. the same instruction
jmp start
