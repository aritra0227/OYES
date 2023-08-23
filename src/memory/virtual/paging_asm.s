global set_pdt
global flush_tlb_entry


set_pdt:
    mov eax, [esp+4]
    and eax, 0xFFFFF000
    or eax, 0x0c ;ENABLE PAGE WRITE THROUGH AND U/S
    mov cr3, eax
    ret

flush_tlb_entry:
    mov eax, [esp+4]
    invlpg [eax]
    ret