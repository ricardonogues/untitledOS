#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../include/mm/memory_map.h"

struct memory_region memory_map[MAX_MEMORY_REGIONS];
uint32_t memory_map_entries;

void init_memory_map()
{
    memset(memory_map, 0, sizeof(memory_map));
    memory_map_entries = 0;

    // Read memory map from where the bootloader placed it
    uint16_t memory_map_entry_count = *(uint16_t *)MEMORY_MAP_ADDRESS;

    printf("Memory map entry count: %d\n", memory_map_entry_count);

    if (memory_map_entry_count == 0)
    {
        printf("Failed to initialize memory map: Invalid address.\n");
        return;
    }

    if (memory_map_entry_count > MAX_MEMORY_REGIONS)
    {
        printf("Memory map entry count exceeds maximum allowed.\n");
        return;
    }

    struct memory_region *map_ptr = (struct memory_region *)(MEMORY_MAP_ADDRESS + sizeof(uint16_t)); // Skip the first 2 bytes which may contain the number of entries
    for (uint32_t i = 0; i < memory_map_entry_count; i++)
    {
        add_memory_region(map_ptr[i].base_addr, map_ptr[i].length, map_ptr[i].type);
    }
}

void add_memory_region(uint64_t base_addr, uint64_t length, uint32_t type)
{
    if (memory_map_entries < MAX_MEMORY_REGIONS)
    {
        memory_map[memory_map_entries].base_addr = base_addr;
        memory_map[memory_map_entries].length = length;
        memory_map[memory_map_entries].type = type;
        memory_map_entries++;
    }
}

void print_memory_map()
{
    for (uint32_t i = 0; i < memory_map_entries; i++)
    {
        struct memory_region *region = &memory_map[i];
        printf("Memory Region: %d", i);
        printf(" Base Address: %p", (void *)region->base_addr);
        printf(" Length: %d", region->length);
        printf(" Type: %u", region->type);
        printf("\n");
    }
}