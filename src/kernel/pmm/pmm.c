#include "../include/pmm/pmm.h"
#include "../include/mm/memory_map.h"
#include <stdio.h>
#include <string.h>

static uint32_t *bitmap = NULL;
static uint32_t total_pages = 0;
static uint32_t free_pages = 0;
static uint32_t used_pages = 0;
static uintptr_t managed_base = 0;
static uint32_t bitmap_size = 0;

#define BITMAP_SET(bitmap, index) (bitmap[(index) / 32] |= (1 << ((index) % 32)))
#define BITMAP_CLEAR(bitmap, index) (bitmap[(index) / 32] &= ~(1 << ((index) % 32)))
#define BITMAP_TEST(bitmap, index) (bitmap[(index) / 32] & (1 << ((index) % 32)))

static inline uint32_t addr_to_page(uintptr_t addr)
{
    return (addr - managed_base) / PAGE_SIZE;
}

static inline uintptr_t page_to_addr(uint32_t page)
{
    return managed_base + (page * PAGE_SIZE);
}

static uint32_t find_free_page()
{
    for (uint32_t i = 0; i < total_pages; i++)
    {
        if (!BITMAP_TEST(bitmap, i))
        {
            return i;
        }
    }
    return -1; // No free page found
}

static uint32_t find_free_pages(size_t count)
{
    if (count == 0 || count > free_pages)
    {
        return -1; // Invalid request or not enough free pages
    }

    uint32_t consecutive = 0;
    for (uint32_t i = 0; i < total_pages - count; i++)
    {
        uint32_t j;
        for (j = 0; j < count; j++)
        {
            if (BITMAP_TEST(bitmap, i + j))
            {
                break;
            }
        }
        if (j == count)
        {
            return i; // Return the start index of the free pages
        }
    }
    return -1; // No sufficient free pages found
}

void pmm_init(void)
{
    init_memory_map();

    uint64_t largest_base = 0;
    uint64_t largest_size = 0;
    uint64_t bitmap_location = 0;

    for (uint32_t i = 0; i < memory_map_entries; i++)
    {
        struct memory_region *region = &memory_map[i];
        if (region->type == MEMORY_TYPE_AVAILABLE)
        {
            if (region->length > largest_size)
            {
                largest_base = region->base_addr;
                largest_size = region->length;
            }
        }
    }

    if (largest_size == 0)
    {
        printf("No available memory regions found.\n");
        return;
    }

    managed_base = PAGE_ALIGN(largest_base);
    total_pages = (largest_size - (managed_base - largest_base)) / PAGE_SIZE;
    bitmap_size = (total_pages + 31) / 32 * sizeof(uint32_t);
    bitmap_location = managed_base;
    bitmap = (uint32_t *)bitmap_location;

    memset(bitmap, 0, bitmap_size);

    uint32_t bitmap_pages = (bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = 0; i < bitmap_pages; i++)
    {
        BITMAP_SET(bitmap, i);
    }

    free_pages = total_pages - bitmap_pages;
    used_pages = bitmap_pages;

    printf("PMM initialized:\n");
    printf("  Base Address: %p\n", (void *)managed_base);
    printf("  Total Pages: %u\n", total_pages);
    printf("  Free Pages: %u\n", free_pages);
    printf("  Used Pages: %u\n", used_pages);
}

void *pmm_alloc_page(void)
{
    if (free_pages == 0)
    {
        printf("No free pages available.\n");
        return NULL;
    }

    uint32_t page_index = find_free_page();
    if (page_index == -1)
    {
        printf("No free pages available.\n");
        return NULL;
    }

    BITMAP_SET(bitmap, page_index);
    free_pages--;
    used_pages++;
    uintptr_t page_addr = page_to_addr(page_index);
    pmm_mark_page_used(page_addr);

    return (void *)page_addr;
}

void *pmm_alloc_pages(size_t count)
{
    if (count == 0 || count > free_pages)
    {
        printf("Invalid request for page allocation.\n");
        return NULL;
    }

    uint32_t start_index = find_free_pages(count);
    if (start_index == -1)
    {
        printf("No sufficient free pages available.\n");
        return NULL;
    }

    for (size_t i = 0; i < count; i++)
    {
        BITMAP_SET(bitmap, start_index + i);
    }

    free_pages -= count;
    used_pages += count;

    uintptr_t page_addr = page_to_addr(start_index);
    pmm_mark_pages_used((uintptr_t *)(page_addr), count);

    return (void *)page_addr;
}

void pmm_free_page(void *page)
{
    if (page == NULL)
    {
        printf("Attempt to free a NULL page.\n");
        return;
    }

    uintptr_t page_addr = (uintptr_t)page;
    uint32_t page_index = addr_to_page(page_addr);

    if (page_index >= total_pages || !BITMAP_TEST(bitmap, page_index))
    {
        printf("Attempt to free an invalid or already freed page.\n");
        return;
    }

    BITMAP_CLEAR(bitmap, page_index);
    free_pages++;
    used_pages--;
}

void pmm_free_pages(void *pages, size_t count)
{
    if (pages == NULL || count == 0)
    {
        printf("Invalid request to free pages.\n");
        return;
    }

    uintptr_t page_addr = (uintptr_t)pages;
    uint32_t start_index = addr_to_page(page_addr);

    for (size_t i = 0; i < count; i++)
    {
        uint32_t page_index = start_index + i;

        if (page_index >= total_pages || !BITMAP_TEST(bitmap, page_index))
        {
            printf("Attempt to free an invalid or already freed page at index %u.\n", page_index);
            continue;
        }

        BITMAP_CLEAR(bitmap, page_index);
    }

    free_pages += count;
    used_pages -= count;
}

void pmm_mark_page_used(uintptr_t page_addr)
{
    uint32_t page_index = addr_to_page(page_addr);
    if (!BITMAP_TEST(bitmap, page_index) && page_index < total_pages)
    {
        BITMAP_SET(bitmap, page_index);
        used_pages++;
        free_pages--;
    }
}

void pmm_mark_pages_used(uintptr_t *page_addrs, size_t count)
{
    if (page_addrs == NULL || count == 0)
    {
        printf("Invalid request to mark pages as used.\n");
        return;
    }

    for (size_t i = 0; i < count; i++)
    {
        pmm_mark_page_used(page_addrs[i]);
    }
}

void pmm_get_info(pmm_info_t *info)
{
    if (info == NULL)
    {
        printf("Invalid PMM info pointer.\n");
        return;
    }

    info->bitmap = bitmap;
    info->total_pages = total_pages;
    info->free_pages = free_pages;
    info->used_pages = used_pages;
    info->base_address = managed_base;
    info->bitmap_size = bitmap_size;
}

void pmm_print_stats(void)
{
    printf("PMM Statistics:\n");
    printf("  Total Pages: %u\n", total_pages);
    printf("  Free Pages: %u\n", free_pages);
    printf("  Used Pages: %u\n", used_pages);
    printf("  Base Address: %p\n", (void *)managed_base);
    printf("  Bitmap Size: %u bytes\n", bitmap_size);
}