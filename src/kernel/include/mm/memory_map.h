#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <stdint.h>

#define MAX_MEMORY_REGIONS 128
#define MEMORY_MAP_ADDRESS 0x8000

struct memory_region
{
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t extended_attributes; // Additional attributes for future use
} __attribute__((packed));

extern struct memory_region memory_map[MAX_MEMORY_REGIONS];
extern uint32_t memory_map_entries;

void init_memory_map();
void add_memory_region(uint64_t base_addr, uint64_t length, uint32_t type);
void print_memory_map();

#endif // MEMORY_MAP_H