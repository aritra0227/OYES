/**
 * PAGE FRAME ALLOCATION IMPLEMENTATION:
 * USING A HYBRID METHOD: BITMAP + STACK
 * UTILIZE BITMAP TO MARK PAGES THAT ARE CURRENTLY ON THE STACK
 * MAINTAIN ATMOST 1024 PAGES ON THE STACK
 * PUSH FREE PAGES ON THE STACK, POP PAGES WHEN USED
 * WHEN STACK IS EMPTY, ALLOCATE MORE PAGES THROUGH BITMAP
*/

#include "memory/page_frame_alloc.h"
#include "utility/utilities.h"

#define MEMORY_END 0xFFFFFFFF
#define PT_ENTRIES 1024
#define PD_ENTRIES 1024
#define FOUR_KB 0x1000
#define TOTAL_PAGE_FRAME_NUMS (PT_ENTRIES * PD_ENTRIES)
#define BITMAP_ENTRIES TOTAL_PAGE_FRAME_NUMS / 32
#define ONE_KB_OF_PAGES_BITMAP_ENTRIES 32 // 1024(1 KB)/32(EACH ENTRY OF BITMAP RESPONSIBLE FOR 32 PFs) = 32
#define ONES_FOUR_BYTES 0b11111111111111111111111111111111
#define MAX_REQUESTED_FRAMES 1024
// NOTE THE FOLLOWING TWO MACROS DO NOT OVERLAP WITH ANY PHYS ADDRESSES (SINCE START OFFSET IS HIGHER THAN THE SIZE OF THE KERNEL a.k.a >= 0x1)
#define STACK_OP_FAIL 0
#define STACK_OP_SUCCESS 1

struct BITMAP
{
    unsigned int start_offset;
    unsigned int bit_map[BITMAP_ENTRIES]; //bit_map[i] is responsible for 32 pageframes;;;
    unsigned int last_allocated_index;    //treat it as a ring
} __attribute__((aligned));

struct STACK
{
    int max_len_index;
    int top_index;
    unsigned int stack[PT_ENTRIES]; //allocating 1024 pages at a time
} __attribute__((aligned));

/**
 * Pushes item onto stack
*/
unsigned int stack_push(struct STACK *stack, unsigned int item)
{
    if (stack->top_index < stack->max_len_index)
    {
        stack->stack[++stack->top_index] = item;
        return STACK_OP_SUCCESS;
    }
    else
        return STACK_OP_FAIL;
}
/**
 * pops returns the top item
*/
unsigned int stack_tpop(struct STACK *stack)
{
    if (stack->top_index >= 0)
    {
        return stack->stack[stack->top_index--];
    }
    else
        return STACK_OP_FAIL;
}

static struct BITMAP PAGE_FRAME_ALLOCATOR; //use bitmap to allocate pages
static struct STACK FREE_PAGE_FRAMES;      //use stack to keep track of free pages;;; Should contain physical addresses

void page_frame_allocator_init(unsigned int kernel_physical_end)
{
    PAGE_FRAME_ALLOCATOR.start_offset = (divideUnsigned(kernel_physical_end, FOUR_KB) + 1) * FOUR_KB;
    //set all entries of bitmap to 0
    for (int i = 0; i < BITMAP_ENTRIES; ++i)
        PAGE_FRAME_ALLOCATOR.bit_map[i] = (unsigned int)0; //probably redundant since variable is static??
    //set first 1024 pages to be allocated:
    for (int i = 0; i < ONE_KB_OF_PAGES_BITMAP_ENTRIES; ++i)
    {
        PAGE_FRAME_ALLOCATOR.bit_map[i] = ONES_FOUR_BYTES;
    }
    PAGE_FRAME_ALLOCATOR.last_allocated_index = ONE_KB_OF_PAGES_BITMAP_ENTRIES - 1;
    //initialize stack:
    FREE_PAGE_FRAMES.top_index = -1;
    FREE_PAGE_FRAMES.max_len_index = PT_ENTRIES - 1;
    //push first 1024 page frames onto stack:
    for (int i = 0; i < PT_ENTRIES; ++i)
        stack_push(&FREE_PAGE_FRAMES, PAGE_FRAME_ALLOCATOR.start_offset + (0x1000 * i));
    return;
}

/**
 * populates FREE_PAGE_FRAMES with 1024 free page addresses
 * Mark corresponding pages on PAGE_FRAME_ALLOCATOR as 1 (used)
 * Only call when FREE_PAGE_FRAMES is empty; meaning all free pages in the stack are used
*/
unsigned int bitmap_get_unallocated_pages()
{
    unsigned int pages_allocated = 0;
    unsigned int curr_index = PAGE_FRAME_ALLOCATOR.last_allocated_index;
    unsigned int start_index = PAGE_FRAME_ALLOCATOR.last_allocated_index;
    unsigned int curr_page_address;
    while (pages_allocated < PT_ENTRIES)
    {
        for (int i = 0; i < 32; ++i)
        {
            if (!((1 << i) & PAGE_FRAME_ALLOCATOR.bit_map[curr_index]))
            {                                                         //recall 0 bit indicates free page
                PAGE_FRAME_ALLOCATOR.bit_map[curr_index] |= (1 << i); //mark as allocated page
                curr_page_address = PAGE_FRAME_ALLOCATOR.start_offset + (32 * FOUR_KB * curr_index) + (i * FOUR_KB);
                stack_push(&FREE_PAGE_FRAMES, curr_page_address);
                pages_allocated += 1;
            }
            if (pages_allocated == 1024)
            {
                PAGE_FRAME_ALLOCATOR.last_allocated_index = curr_index;
                return 1024;
            }
        }
        curr_index += 1;
        if (curr_index == BITMAP_ENTRIES)
            curr_index = 0; //treating as a ring
        if (curr_index == start_index)
        {
            PAGE_FRAME_ALLOCATOR.last_allocated_index = curr_index;
            return pages_allocated;
        }
    }
    PAGE_FRAME_ALLOCATOR.last_allocated_index = curr_index;
    return 1024;
}

/**
 * Adds free page to the stack if stack is not full
 * Otherwise, marks corresponding bitmap entry as free
*/
void free_page_frame(unsigned int page_address)
{
    if (FREE_PAGE_FRAMES.top_index < FREE_PAGE_FRAMES.max_len_index)
    {
        stack_push(&FREE_PAGE_FRAMES, page_address);
    }
    else
    {
        //mark bitmap entry as free
        page_address -= PAGE_FRAME_ALLOCATOR.start_offset;
        unsigned int nth_page = divideUnsigned(page_address, FOUR_KB);
        unsigned int bitmap_index = divideUnsigned(nth_page, 32); // since each index covers 32 page frames
        unsigned int bitmap_bit = modulo(nth_page, 32);
        unsigned int mask = (1 << bitmap_bit) ^ ONES_FOUR_BYTES;
        PAGE_FRAME_ALLOCATOR.bit_map[bitmap_index] &= mask;
    }
    return;
}

/**
 * Allocates num_page_frames frames
 * @param: num_page_frames: desired number of page frames to allocate
 * Note: Each call returns MAX_REQUESTED_FRAMES frames at most
*/
unsigned int *pf_allocate_frames(unsigned int num_page_frames)
{
    static unsigned int allocated_page_frames[MAX_REQUESTED_FRAMES];
    for (unsigned int i = 0; i < num_page_frames; ++i)
    {
        //first exhaust the stack and if required allocate more frames from bitmap
        unsigned int res = stack_tpop(&FREE_PAGE_FRAMES);
        if (res != STACK_OP_FAIL)
            allocated_page_frames[i] = res;
        if (FREE_PAGE_FRAMES.top_index == -1)
        {
            unsigned int pages_allocated = bitmap_get_unallocated_pages();
            if (pages_allocated < (1024 - i + 1))
            {
                return 0; // meaning it wasn't able to allocate desired number of frames
            }
        }
    }
    return allocated_page_frames;
}