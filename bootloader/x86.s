bits 16

global x86_div64_32
x86_div64_32:

    ; make new call frame
    push bp     ; save old call frame
    mov bp, sp  ; initialize new call frame

    push bx 
    ; divide upper 32 bits
    mov eax, [bp + 8] ; eax <- upper 32 bits of dividend // numbers are stored in little endian, aka. lower bits -> lower address
    mov ecx, [bp + 12] ; ecx <- divisor
    xor edx, edx
    div ecx             ; eax- quot, edx -remainder

    ; store upper 32 bits  of quotient
    mov bx, [bp + 16]
    mov [bx + 4], eax

    ;divide lower 32 bits
    mov eax, [bp + 4] ; eax <- lower 32 bits of dividend
                    ; edx <- old remainder
    div ecx

    ;store results
    mov [bx], eax
    mov bx, [bp + 18]
    mov [bx], edx

    pop bx
    ;restore old call frame
    mov sp, bp
    pop bp
    ret