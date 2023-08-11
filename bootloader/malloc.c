#include "malloc.h"
#include "constants.h"
#include "utilities.h"
#include "paging.h"
#include "page_frame_alloc.h"

#define PAGE_FRAME_SIZE 0x1000
#define NALLOC 1024

//free-list implementation (basically a linked list)
struct HEADER
{
    struct HEADER *next_header;
    unsigned int size;
};
typedef struct HEADER HEADER_T;

static HEADER_T *freep = 0; //start of freelist
static HEADER_T base;
static unsigned int malloc_start_vaddress = 0x400000; //start at 4MB for now
static unsigned int total_malloc_pages = 0;

static void *get_more_memory(unsigned int nunits);

void *malloc(unsigned int nbytes)
{
    if (nbytes == 0)
        return NULL;
    if (freep == 0)
    {
        base.next_header = freep = &base;
        base.size = 0;
    }
    HEADER_T *p, *prevp;
    prevp = freep;
    unsigned int nunits = div_absolute_ceil(nbytes, sizeof(HEADER_T));
    for (p = prevp->next_header;; prevp = p, p = p->next_header)
    {
        if (p->size >= nunits)
        {
     
            if (p->size == nunits) 
                prevp->next_header = p->next_header;
            else
            {
                p->size -= nunits;                
                p += p->size;
                p->size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }
        if (p == freep)
        {
            if ((p = get_more_memory(nunits)) == NULL)
                return NULL;
        }
    }
}

static void *get_more_memory(unsigned int nunits)
{
    if(nunits < NALLOC) nunits= NALLOC;
    HEADER_T *p;
    unsigned int num_page_frames = div_ceil(NALLOC * sizeof(HEADER_T), PAGE_FRAME_SIZE);
    unsigned int total_size = num_page_frames * PAGE_FRAME_SIZE;
    unsigned int *malloc_paddress = pf_allocate_frames(num_page_frames);
    // unsigned int malloc_vaddress = pdt_kernel_find_next_vaddr(total_size); // very slowww
    unsigned int malloc_vaddress = malloc_start_vaddress+total_malloc_pages*PAGE_FRAME_SIZE; // DUMMY METHOD WHICH STARTS ALLOCATING MEMORY FROM THE BEGINNING
    total_malloc_pages += num_page_frames;
    for (unsigned int i = 0, virt = malloc_vaddress; i < num_page_frames; ++i, virt += PAGE_FRAME_SIZE)
    {
        
       
        map_page((unsigned int *)malloc_paddress[i], (unsigned int *)virt);
        unsigned int tmp_entry = kernel_get_temporary_entry();
        unsigned int *page = get_page(virt);
        SET_ATTR(page, PTE_RW);
        SET_ATTR(page, PTE_PRESENT);
        kernel_set_temporary_entry(tmp_entry);
    }
    p = (HEADER_T *)malloc_vaddress;
    p->size = divideUnsigned(total_size, sizeof(HEADER_T));
    free((void*)(p+1));
    return freep;
}

void free(void *ap)
{
    if (ap == 0)
        return;
    HEADER_T *bp, *p;
    bp = (HEADER_T *)ap - 1;
    for (p = freep; !(bp > p && bp < p->next_header); p = p->next_header){
        if (p >= p->next_header && (bp > p || bp < p->next_header))
            break;
    }
    if (bp + bp->size == p->next_header)
    {
        bp->size += p->next_header->size;
        bp->next_header = p->next_header->next_header;
    }
    else
        bp->next_header = p->next_header;
    if (p + p->size == bp)
    {
        p->size += bp->size;
        p->next_header = bp->next_header;
    }
    else
        p->next_header = bp;
    freep = p;
}