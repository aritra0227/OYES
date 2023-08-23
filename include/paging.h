#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H

#define GET_PD_IDX(virt_address) (virt_address >> 22)           //recall highest 10 bits are pd entry
#define GET_PT_IDX(virt_address) ((virt_address >> 12) & 0x3FF) //middle 10 bits are pt entry
#define GET_PAGE_PHYS_ADDRESS(pd_entry) ((*pd_entry) & ~0xFFF)  //recall highest 20 bits represent physical address
#define SET_ATTR(entry, attr) (*entry |= attr)
#define CLEAR_ATTR(entry, attr) (*entry &= ~attr)
#define CHECK_ATTR(entry, attr) (*entry & attr) //to check whether some attribute exists
#define SET_FRAME(entry, address) (*entry = (*entry & ~0xFFFFF000) | address)
#define PDT_IDX_TO_VIRTUAL(a) (((a) << 22))
#define PT_IDX_TO_VIRTUAL(a) (((a) << 12))

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
void paging_init(unsigned int kernel_pd, unsigned int kernel_pt);
unsigned int kernel_map_page(unsigned int *paddress, unsigned int *vaddress, unsigned int is_user);
unsigned int map_page(unsigned int *pd, unsigned int *paddress, unsigned int *vaddress, unsigned int is_user);
void unmap_page(unsigned int *vaddress);
unsigned int *get_page(unsigned int vaddress);
unsigned int *allocate_page(unsigned int *pte);
void free_page(unsigned int *pte);
unsigned int pdt_kernel_find_next_vaddr(unsigned int size);
unsigned int kernel_get_temporary_entry();
unsigned int *create_pdt(unsigned int *pdt_paddr);
void kernel_set_temporary_entry(unsigned int entry);
void load_process_pdt(unsigned int *pdt, unsigned int pdt_paddr);

#endif