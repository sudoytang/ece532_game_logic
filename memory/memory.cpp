#include "memory.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "tlsf/tlsf.h"

static void* heap = nullptr;
static tlsf_t tlsf;

#if defined(__MICROBLAZE__)
// predefined big array for memory allocation
void* _mb_memory_pool = (void*)0x84000000;
#endif


int memory_init() {
#if defined(__MICROBLAZE__)
    heap = (void*)_mb_memory_pool;
    auto on_error = +[](){};
#else
    heap = malloc(HEAP_SIZE);
    if (heap == nullptr) {
        return -1;
    }
    auto on_error = +[]() {
        if (tlsf) free(heap);
    };
#endif
    memset(heap, 0, HEAP_SIZE);
    tlsf = tlsf_create_with_pool(heap, HEAP_SIZE);
    if (tlsf == nullptr) {
    	on_error();
    	return -1;
    }
    return 0;
}

void memory_end() {
#if defined(__MICROBLAZE__)
    heap = nullptr;
#else
    free(heap);
    heap = nullptr;
#endif
    return;
}


//#define MEMORY_DEBUG
#ifdef MEMORY_DEBUG
#define print_memalloc(size, addr) xil_printf("Allocated %d @%x\n", size, addr);
#define print_memfree(addr) xil_printf("Freed @%x\n", addr);
#define print_vmemalloc(size, addr) xil_printf("Allocated VMEM %d @%x\n", size, addr);
#define print_vmemfree(addr) xil_printf("Freed VMEM @%x\n", addr);
#define print_memrealloc(size, old_addr, new_addr) xil_printf("Reallocated %d @%x->@%x\n", size, old_addr, new_addr);
#define print_vmemrealloc(size, old_addr, new_addr) xil_printf("Reallocated VMEM %d @%x->@%x\n", size, old_addr, new_addr);
#else
#define print_memalloc(size, addr)
#define print_memfree(addr)
#define print_vmemalloc(size, addr)
#define print_vmemfree(addr)
#define print_memrealloc(size, old_addr, new_addr)
#define print_vmemrealloc(size, old_addr, new_addr)
#endif



void* allocate(uint32_t size) {
    if (tlsf == nullptr || heap == nullptr) {
    	xil_printf("[MEMORY ERROR] Trying to allocate %d while heap is not available!\n", size);
        return nullptr;
    }
    auto res = tlsf_malloc(tlsf, size);
    print_memalloc(size, res);
    return res;
}

void* allocate_aligned(uint32_t size, uint32_t alignment) {
    if (tlsf == nullptr || heap == nullptr) {
    	xil_printf("[MEMORY ERROR] Trying to allocate %d while heap is not available!\n", size);
        return nullptr;
    }
    auto res = tlsf_memalign(tlsf, alignment, size);
    print_memalloc(size, res);
    return res;
}

void deallocate(void* ptr) {
    if (tlsf == nullptr || heap == nullptr) {
    	xil_printf("[MEMORY ERROR] Trying to deallocate [%x] while heap is not available!\n", ptr);
        return;
    }
    tlsf_free(tlsf, ptr);
    print_memfree(ptr);
}

void* reallocate(void* ptr, uint32_t size) {
    if (tlsf == nullptr || heap == nullptr) {
    	xil_printf("[MEMORY ERROR] Trying to reallocate %d at [%x] while heap is not available!\n", size, ptr);
        return nullptr;
    }
    auto res = tlsf_realloc(tlsf, ptr, size);
    print_memrealloc(size, ptr, res);
    return res;
}


