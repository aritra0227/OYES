#ifndef INCLUDE_PAGING_ASM_H
#define INCLUDE_PAGING_ASM_H

/**
 * sets current page directory w.r.t a process
*/
void set_pdt(unsigned int * pd_address);

/**
 * invalidates a single page from the tlb
*/
void flush_tlb_entry(unsigned int pte_address);

#endif