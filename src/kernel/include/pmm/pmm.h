#ifndef PMM_H
#define PMM_H

#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096 // Size of a memory page in bytes
#define PAGE_ALIGN(addr) ((addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(addr) (addr & ~(PAGE_SIZE - 1))

#define MEMORY_TYPE_AVAILABLE 1
#define MEMORY_TYPE_RESERVED 2
#define MEMORY_TYPE_ACPI 3
#define MEMORY_TYPE_NVS 4
#define MEMORY_TYPE_BADRAM 5

typedef struct
{
    uint32_t *bitmap;       // Pointer to the bitmap for tracking allocated pages
    uint32_t total_pages;   // Total number of pages in the bitmap
    uint32_t free_pages;    // Number of free pages available
    uint32_t used_pages;    // Number of pages currently in use
    uintptr_t base_address; // Base address of the physical memory managed by this PMM
    uint32_t bitmap_size;   // Size of the bitmap in bytes
} pmm_info_t;

void pmm_init(void);
void *pmm_alloc_page(void);
void *pmm_alloc_pages(size_t count);
void pmm_free_page(void *page);
void pmm_free_pages(void *pages, size_t count);
void pmm_get_info(pmm_info_t *info);
void pmm_mark_page_used(uintptr_t page_addr);
void pmm_mark_pages_used(uintptr_t *page_addrs, size_t count);
void pmm_print_stats(void);

#endif /* PMM_H */