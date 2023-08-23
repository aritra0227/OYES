#ifndef INCLUDE_PAGE_FRAME_ALLOC_H
#define INCLUDE_PAGE_FRAME_ALLOC_H


void page_frame_allocator_init(unsigned int kernel_physical_end);
void free_page_frame(unsigned int page_address);
unsigned int *pf_allocate_frames(unsigned int num_page_frames);

#endif