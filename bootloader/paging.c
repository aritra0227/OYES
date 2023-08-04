#include "paging.h"
#include "stdio.h"
#include "page_frame_alloc.h"
#include "paging_asm.h"

#define PAGE_TABLE_ENTRIES 1024
#define PAGE_DIRECTORY_ENTRIES 1024
#define FOUR_KB 0x1000

#define GET_PD_IDX(virt_address) (virt_address >> 22)              //recall highest 10 bits are pd entry
#define GET_PT_IDX(virt_address) ((virt_address >> 12) & 0x3FF)    //middle 10 bits are pt entry
#define GET_PAGE_PHYS_ADDRESS(pd_entry) ((*pd_entry) & 0xFFFFF000) //recall highest 20 bits represent physical address
#define SET_ATTR(entry, attr) (*entry |= attr)
#define CLEAR_ATTR(entry, attr) (*entry &= ~attr)
#define CHECK_ATTR(entry, attr) (*entry &= attr) //to check whether some attribute exists
#define SET_FRAME(entry, address) (*entry = (*entry & 0xFFF) | (*address & 0xFFFFF000))

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

typedef enum
{
    PTE_PRESENT = 0X01,
    PTE_RW = 0X02,
    PTE_USER = 0x04,
    PTE_WRITE_THROUGH = 0x08,
    PTE_CACHE_DISABLE = 0x10,
    PTE_ACCESSED = 0x20,
    PTE_DIRTY = 0x40,
    PTE_PAT = 0x80,
    PTE_GLOBAL = 0x100,
    PTE_FRAME = 0xFFFFF000,
} PT_FLAGS;

typedef enum
{
    PDE_PRESENT = 0X01,
    PDE_RW = 0X02,
    PDE_USER = 0x04,
    PDE_WRITE_THROUGH = 0x08,
    PDE_CACHE_DISABLE = 0x10,
    PDE_ACCESSED = 0x20,
    PDE_DIRTY = 0x40,   // note: only used for 4mb entries
    PDE_PS = 0x80,      // note: page size = 0 for 4kb and =1 for 4MB
    PDE_GLOBAL = 0x100, // note: 4MB entry only
    PDE_PAT = 0X2000,   // note: 4MB entry only
    PDE_FRAME = 0xFFFFF000,
} PD_FLAGS;

static struct PDE *CURRENT_PD;
// static struct PTE *CURRENT_PT;

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
    unsigned int *pte = (unsigned int *)((unsigned int *)CURRENT_PD + GET_PT_IDX(vaddress));
    unsigned int *pt_paddress = (unsigned int *)GET_PAGE_PHYS_ADDRESS(pte);
    unsigned int *page = pt_paddress + GET_PT_IDX(vaddress);
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
        SET_FRAME(pte, page_frame);
        SET_ATTR(pte, PTE_PRESENT); //mark it as present
    }
    return (unsigned int *)page_frame[0]; //a little weird, but pf_allocate_frames returns an array
}

/**
 * Free one page of memory
*/
void free_page(unsigned int *pte)
{
    free_page_frame(GET_PAGE_PHYS_ADDRESS(pte));
    CLEAR_ATTR(pte, PTE_PRESENT);
}

//write asm function for loading new pd (remember each process will have its own pd) --> done
//note: reloading the address of the current pd into the cr3 register will invalidate all its current entries
//write asm function for invlpg to invalidate any particular page --> done

unsigned int map_page(unsigned int paddress, unsigned int vaddress)
{
    unsigned int *pde = get_pde((unsigned int *)CURRENT_PD, vaddress);
    if (pde && (!CHECK_ATTR(pde, PDE_PRESENT)))
    {
        // if page table not present in PD:
        unsigned int *frame = pf_allocate_frames(1);
        if (!frame)
            return 0;                              //out of memory
        unsigned int *pt = (unsigned int *)*frame; // first entry points to phys address of block,
                                                   //hence casting that to a pointer sets the address of pt
        //clear pt:
        for (unsigned int i = 0; i < FOUR_KB; ++i)
        {
            pt[i] = 0;
        }
        SET_ATTR(pde, PDE_PRESENT);
        SET_ATTR(pde, PDE_RW);
        SET_FRAME(pde, pt);
    }
    unsigned int *pt = (unsigned int *)GET_PAGE_PHYS_ADDRESS(pde);
    unsigned int *pte = get_pte(pt, vaddress);
    SET_FRAME(pte, &paddress);
    SET_ATTR(pte, PTE_PRESENT);
    return 1; //1 for true
}

void unmap_page(unsigned int vaddress)
{
    unsigned int *pte = get_page(vaddress);
    SET_FRAME(pte, (unsigned int *)0);
    CLEAR_ATTR(pte, PTE_PRESENT);
}

void set_current_pd(unsigned int pd_address)
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
    (void)kernel_pt;
    // unsigned int *pages = pf_allocate_frames(1024);
    // for(int i=0; i< 10; ++i) printf(" first set of addy: %u", pages[i]);
    // unsigned int *pages2 = pf_allocate_frames(1024);
    // for(int i=0; i< 1024; ++i) printf(" second set of addy: %u", pages2[i]);
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
    //             printf("    couldn't allocate page");
    //         }
    //     }
    // }
    printf("address of pd: %u", kernel_pd);
    // set_current_pd(kernel_pd);
    CURRENT_PD = (struct PDE *)kernel_pd;
    printf("this is the first allocated page");
    return;
}