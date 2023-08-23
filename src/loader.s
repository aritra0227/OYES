;This file will load the kernel in the higher half
;Refer to https://wiki.osdev.org/Higher_Half_bare_bones and https://wiki.osdev.org/Paging#Page_Directory

global loader                                        ; the entry symbol for ELF
global kernel_stack
extern kmain                                         ; function is defined elsewhere
extern kernel_physical_start
extern kernel_physical_end
extern kernel_virtual_start
extern kernel_virtual_end

MAGIC_NUMBER    equ 0x1BADB002                       ; define the magic number constant
ALIGN_MODULES   equ 1 << 0                           ; tell GRUB to align modules
MEMINFO         equ 1 << 1        
FLAGS           equ ALIGN_MODULES | MEMINFO          ; multiboot flags
CHECKSUM        equ -(MAGIC_NUMBER + FLAGS)          ; calculate the checksum
                                                     ; (magic number + checksum + flags should equal 0)

KERNEL_STACK_SIZE    equ    4096	                         ; size of stack in bytes
VADDR_OFFSET         equ    0xC0000000
HIGHER_KERNEL_PD_IDX equ VADDR_OFFSET >> 22 ; recall higher 10 bits refer to the PDE index
                                            ; (, then the next 10 refer to the PTE) 
FOUR_KB              equ 0x1000
PT_ENTRIES           equ 1024
                                            
PDE_FIRST_4MB_IDNTY_MAP_CONFIG      equ 0x8f ;also setting U/S 0x8b           ; set bits: P, R/W, PWT and most importantly PS (to easily map the first 4MB)
PDE_HIGHER_KERNEL_PHYS_MAP_CONFIG   equ 0xf  ;...same here... 0x0b           ; set bits: P, R/W, PWT

section .bss
align 4				        ; align at 4 bytes
kernel_stack:			    ; label points to beginning of memory
	resb KERNEL_STACK_SIZE  ; reserve stack for the kernel

; pre-allocate pages for pdt and pt 
section .data
align 4096                  ; align at 4096 bytes = 4 KB
boot_page_directory:
    times 1024 dd 0 ;each dd (double word) equals 4 bytes, hence 1024*4 = 4096
boot_page_table:
    times 1024 dd 0

section .data
align 4
multiboot_module_info:
    dd 0

section .text                   ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd FLAGS                    ; write the align modules instruction
    dd CHECKSUM                 ; and the checksum

loader:                                         ; the loader label (defined as entry point in linker script)
    mov ecx, (multiboot_module_info - VADDR_OFFSET)
    mov [ecx], ebx
    ; now first set up page directory table
    ; first PDE must be identity mapped with PS=1 (hence simply id mapping the whole first 4MB)
    mov eax, boot_page_directory - VADDR_OFFSET ; recall during linking process, the base address is set to VADDR_OFFSET   
                                                ; note that this suffices even for the (higher bits of the address) since we are mapping 0x0
    mov [eax], DWORD PDE_FIRST_4MB_IDNTY_MAP_CONFIG     

    ; now load page table onto the higher half of memory (768th row of PDT)
    mov eax, (boot_page_directory - VADDR_OFFSET + (HIGHER_KERNEL_PD_IDX*4)) ;eax points to 768th PDE
    mov ebx, boot_page_table - VADDR_OFFSET
    and ebx, 0xFFFFF000                         ; clear out the last 12 bits to overwrite with configuration
    or  ebx, PDE_HIGHER_KERNEL_PHYS_MAP_CONFIG
    mov [eax], ebx
    
    ; now set up page table:
    mov ecx, boot_page_table - VADDR_OFFSET ;contains address to beginning of PT
    mov ebx, 0
    or ebx, PDE_HIGHER_KERNEL_PHYS_MAP_CONFIG
    .fill_table:
        mov [ecx], ebx
        add ecx, 4
        add ebx, FOUR_KB
        cmp ecx, (boot_page_table - VADDR_OFFSET + (PT_ENTRIES - 1)*4)
        jle .fill_table

    
    ;now enable paging:
    mov eax, boot_page_directory - VADDR_OFFSET
    and eax, 0xFFFFF000
    or  eax, 0xc ;enable U/S too;0x08       ; enable PWT
    mov cr3, eax        ; eax has the address of the page directory
    mov ebx, cr4        ; read current cr4
    or  ebx, 0x00000010 ; set PSE (page size extensions; to enable 4MB pages)
    mov cr4, ebx        ; update cr4
    mov ebx, cr0        ; read current cr0
    or  ebx, 0x80000000 ; set PG (paging-enable bit)
    mov cr0, ebx        ; update cr0

    ;finally jump to higher half kernel using jump to label:
    lea ebx, [higher_kernel]
    jmp ebx 

;from here on, code should execute above 0xC0100000
higher_kernel:
    mov [boot_page_directory], DWORD 0  ;flush the id mapping for the first 4mb
    invlpg[0]                           ;flush tlb
    ; mov eax, 0xCAFEBABE                         ; place the number 0xCAFEBABE in the register eax
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the
					                            ; stack 
    ; push ebx ;; for now, turn off reading grub modules
    ;push addresses of kernel phys/virt start/end to stack
    push DWORD [multiboot_module_info]
    push boot_page_directory
    push boot_page_table
    push kernel_virtual_end
    push kernel_virtual_start
    push kernel_physical_end
    push kernel_physical_start
    call kmain                  ; result of function will live in eax register
.loop:
    jmp .loop                   ; loop forever




