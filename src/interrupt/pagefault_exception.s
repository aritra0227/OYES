global retrieve_page_vaddr
global cli_and_halt

retrieve_page_vaddr:
    mov eax, cr2    ;cr2 contains bad vaddress which caused page fault
    ret

cli_and_halt:
    cli
    hlt