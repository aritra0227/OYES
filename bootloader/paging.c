#include "paging.h"
#include "page_frame_alloc.h"
#include "paging_asm.h"
#include "utilities.h"

#define PAGE_TABLE_ENTRIES 1024
#define PAGE_DIRECTORY_ENTRIES 1024
#define FOUR_KB 0x1000
#define KERNEL_TEMP_VADDR 0xC03FF000
#define KERNEL_START_VADDR 0xC0000000
#define KERNEL_PT_PDT_IDX GET_PD_IDX(KERNEL_START_VADDR)
#define KERNEL_TMP_PT_IDX 1023

struct PDE
{
    unsigned char flags;
    unsigned char middle_address_bits_and_avl; // address bits 15-12 and 4 bits for AVL
    unsigned short higher_address_bits;        //address bits 31-16
} __attribute__((aligned));

struct PTE
{
    unsigned char flags;
    unsigned char middle_address_bits_and_avlg; // address bits: 15-12
    unsigned short higher_address_bits;         //address bits 31-16
} __attribute__((aligned));

static struct PDE *KERNEL_PD;
static struct PTE *KERNEL_PT;
static struct PDE *CURRENT_PD;

static unsigned int temp_map_page(unsigned int paddress);

/**
 * Retrieves page table entry for given virtual address
*/
unsigned int *get_pte(unsigned int *pt, unsigned int vaddress)
{
    if (pt)
        return pt + GET_PT_IDX(vaddress);
    return 0;
}

/**
 * Retrieves page directory entry for given virtual address
*/
unsigned int *get_pde(unsigned int *pd, unsigned int vaddress)
{
    if (pd)
        return pd + GET_PD_IDX(vaddress);
    return 0;
}

/**
 * Retrieves page for virtual_address
*/
unsigned int *get_page(unsigned int vaddress)
{
    unsigned int *pde = (unsigned int *)CURRENT_PD + GET_PD_IDX(vaddress);
    unsigned int pt_paddress = GET_PAGE_PHYS_ADDRESS(pde);
    unsigned int *pt_vaddress = (unsigned int *)temp_map_page(pt_paddress);
    unsigned int *page = pt_vaddress + GET_PT_IDX(vaddress);
    return page;
}

/**
 * Allocates 1 page of memory
*/
unsigned int *allocate_page(unsigned int *pte)
{
    unsigned int *page_frame = pf_allocate_frames(1);
    if (page_frame)
    {
        SET_FRAME(pte, (unsigned int)page_frame);
        SET_ATTR(pte, PTE_PRESENT);           //mark it as present
        return (unsigned int *)page_frame[0]; //a little weird, but pf_allocate_frames returns an array
    }
    else
    {
        return 0;
    }
}

/**
 * Free one page of memory
*/
void free_page(unsigned int *pte)
{
    free_page_frame(GET_PAGE_PHYS_ADDRESS(pte));
    CLEAR_ATTR(pte, PTE_PRESENT);
}

unsigned int kernel_get_temporary_entry()
{
    return *((unsigned int *)&KERNEL_PT[KERNEL_TMP_PT_IDX]);
}

void kernel_set_temporary_entry(unsigned int entry)
{
    KERNEL_PT[KERNEL_TMP_PT_IDX] = *((struct PTE *)&entry);
    flush_tlb_entry(KERNEL_TEMP_VADDR);
}

// Function to Temporarily Map a Page Frame
static unsigned int temp_map_page(unsigned int paddress)
{
    unsigned int *pte = get_pte((unsigned int *)KERNEL_PT, KERNEL_TEMP_VADDR);
    SET_ATTR(pte, PTE_PRESENT);
    SET_ATTR(pte, PTE_RW);
    SET_ATTR(pte, PTE_WRITE_THROUGH);
    SET_FRAME(pte, paddress);
    flush_tlb_entry(KERNEL_TEMP_VADDR);
    return KERNEL_TEMP_VADDR;
}

// Function to Unmap a Temporarily Mapped Page Frame
void unmap_temp_page(unsigned int *vaddress)
{
    unsigned int *temp_pt = (unsigned int *)CURRENT_PD + GET_PD_IDX(KERNEL_TEMP_VADDR);
    SET_FRAME(temp_pt, 0);
    flush_tlb_entry(*vaddress);
}

unsigned int map_page(unsigned int *paddress, unsigned int *vaddress)
{
    unsigned int *pde = get_pde((unsigned int *)CURRENT_PD, (unsigned int)vaddress);
    if (!CHECK_ATTR(pde, PDE_PRESENT))
    {
        // if page table not present in PD:
        unsigned int tmp_entry = kernel_get_temporary_entry();
        unsigned int *frame = pf_allocate_frames(1);
        if (!frame)
            return 0; //out of memory
        unsigned int *pt_vaddr = (unsigned int *)temp_map_page(frame[0]);
        memset((void *)pt_vaddr, 0, FOUR_KB);

        SET_ATTR(pde, PDE_PRESENT);
        SET_ATTR(pde, PDE_RW);
        SET_FRAME(pde, (unsigned int)frame[0]);

        unsigned int *pte = get_pte(pt_vaddr, (unsigned int)vaddress);
        SET_ATTR(pte, PTE_PRESENT);
        SET_FRAME(pte, (unsigned int)paddress);
        kernel_set_temporary_entry(tmp_entry);
        return 1; //1 for true
    }
    else
    {
        SET_ATTR(pde, PDE_RW);
        unsigned int tmp_entry = kernel_get_temporary_entry();
        unsigned int pt_paddr = GET_PAGE_PHYS_ADDRESS(pde);
        unsigned int *pt_vaddr = (unsigned int *)temp_map_page(pt_paddr);
        unsigned int *pte = get_pte((unsigned int *)pt_vaddr, (unsigned int)vaddress);
        SET_ATTR(pt_vaddr, PTE_PRESENT);
        SET_ATTR(pte, PTE_PRESENT);
        SET_FRAME(pte, (unsigned int)paddress);
        kernel_set_temporary_entry(tmp_entry);

        return 1; //1 for true
    }
}

void unmap_page(unsigned int *vaddress)
{
    unsigned int *pte = get_page((unsigned int)vaddress);
    SET_FRAME(pte, 0);
    CLEAR_ATTR(pte, PTE_PRESENT);
}

unsigned int pt_kernel_find_next_vaddr(unsigned int pdt_idx, unsigned int *pt, unsigned int size)
{
    unsigned int num_found = 0, org_i;
    unsigned int num_to_find = divideUnsigned(align_up(size, FOUR_KB), FOUR_KB);
    for (unsigned int i = 0; i < PAGE_TABLE_ENTRIES; ++i)
    {
        if (CHECK_ATTR((pt + i), PTE_PRESENT) || (pdt_idx == KERNEL_PT_PDT_IDX && i == KERNEL_TMP_PT_IDX))
        {
            num_found = 0;
        }
        else
        {
            if (num_found == 0)
            {
                org_i = i;
            }
            ++num_found;
            if (num_found == num_to_find)
            {
                return PDT_IDX_TO_VIRTUAL(pdt_idx) | PT_IDX_TO_VIRTUAL(org_i);
            }
        }
    }
    return 0;
}

unsigned int pdt_kernel_find_next_vaddr(unsigned int size)
{
    unsigned int pt_paddress, pt_vaddress, vaddress = 0;
    unsigned int pdt_idx = GET_PD_IDX(KERNEL_START_VADDR);
    for (; pdt_idx < PAGE_DIRECTORY_ENTRIES; ++pdt_idx)
    {
        unsigned int *current_pde = (unsigned int *)(KERNEL_PD + pdt_idx);
        if (CHECK_ATTR(current_pde, PDE_PRESENT))
        {
            unsigned int tmp_entry = kernel_get_temporary_entry();
            pt_paddress = GET_PAGE_PHYS_ADDRESS(current_pde);
            pt_vaddress = temp_map_page(pt_paddress);
            vaddress = pt_kernel_find_next_vaddr(pdt_idx, (unsigned int *)pt_vaddress, size);
            kernel_set_temporary_entry(tmp_entry);
        }
        else
        {
            vaddress = PDT_IDX_TO_VIRTUAL(pdt_idx);
        }
        if (vaddress != 0)
        {
            return vaddress;
        }
    }
    return 0;
}

void set_current_pd(unsigned int *pd_address)
{
    CURRENT_PD = (struct PDE *)pd_address;
    set_pdt(pd_address);
    return;
}

void paging_init(unsigned int kernel_pd, unsigned int kernel_pt)
{
    //prolly first want to mark corresponding pages of kernel_pd and kernel_pt as used
    // if (kernel_pd >= 0xC0100000) printf("kernel pd is virt");
    // if (kernel_pt >= 0xC0100000) printf("kernel pt is virt");
    // (void)kernel_pt;
    // unsigned int *pages = pf_allocate_frames(1024);
    // for(int i=0; i< 10; ++i) printf(" first set of addy: %u", pages[i]);
    // unsigned int *pages2 = pf_allocate_frames(1024);
    // for(int i=0; i< 1024; ++i) printf(" second set of addy: %u", pages2[i]);
    // unsigned int num_pages=0;
    // for (int i = 0; i < 1024; ++i)
    // {
    //     unsigned int *pages = pf_allocate_frames(1024);
    //     if (i > 1014)
    //     {
    //         if (pages)
    //         {
    //             printf("    %d set of addy: %u  ", i, pages[i]);
    //         }
    //         else
    //         {
    //             printf("    couldn't allocate page, number of pages left %u:", (unsigned int)((1024*1024)- num_pages));
    //         }
    //         num_pages+=1024;
    //     }
    // }
    // printf("address of pd: %u", kernel_pd);
    // set_current_pd((unsigned int *)kernel_pd);
    KERNEL_PD = (struct PDE *)kernel_pd;
    CURRENT_PD = (struct PDE *)kernel_pd;
    KERNEL_PT = (struct PTE *)kernel_pt;
    return;
}