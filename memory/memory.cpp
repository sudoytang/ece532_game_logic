#include "memory.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "tlsf/tlsf.h"

static void* heap = nullptr;
static void* vram_heap = nullptr;
static tlsf_t tlsf;
static tlsf_t tlsf_vram;
int memory_init() {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    heap = malloc(HEAP_SIZE);
    vram_heap = malloc(VRAM_SIZE);
    if (heap == nullptr || vram_heap == nullptr) {
    	if (heap) free(heap);
    	if (vram_heap) free(vram_heap);
        return -1;
    }
    auto on_err = []() {
    	if (tlsf) free(heap);
    	if (tlsf_vram) free(vram);
    };

#elif defined(__MICROBLAZE__)
    heap = 		(void*)0x83000000;
    vram_heap = (void*)0x84000000;
    auto on_err = []() {};
#else
    // unsupported platform
    return -1;
#endif

    memset(heap, 0, HEAP_SIZE);
    memset(vram_heap, 0, VRAM_SIZE);
    tlsf = tlsf_create_with_pool(heap, HEAP_SIZE);
    tlsf_vram = tlsf_create_with_pool(vram_heap, VRAM_SIZE);
    if (tlsf == nullptr || tlsf_vram == nullptr) {
    	on_err();
    	return -1;
    }
    return 0;
}

void memory_end() {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    free(heap);

#elif defined(__MICROBLAZE__)
    heap = nullptr;
#else
    // unsupported platform
    return;
#endif
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
        return nullptr;
    }
    auto res = tlsf_malloc(tlsf, size);
    print_memalloc(size, res);
    return res;
}

void* allocate_aligned(uint32_t size, uint32_t alignment) {
    if (tlsf == nullptr || heap == nullptr) {
        return nullptr;
    }
    auto res = tlsf_memalign(tlsf, alignment, size);
    print_memalloc(size, res);
    return res;
}

void* allocate_vram(uint32_t size) {
    if (tlsf_vram == nullptr || vram_heap == nullptr) {
        return nullptr;
    }
    auto res = tlsf_memalign(tlsf_vram, 16, size);
    print_vmemalloc(size, res);
    return res;
}

void deallocate(void* ptr) {
    if (tlsf == nullptr || heap == nullptr) {
        return;
    }
    tlsf_free(tlsf, ptr);
    print_memfree(ptr);
}

void deallocate_vram(void* ptr) {
    if (tlsf_vram == nullptr || vram_heap == nullptr) {
        return;
    }
    tlsf_free(tlsf_vram, ptr);
    print_vmemfree(ptr);
}

void* reallocate(void* ptr, uint32_t size) {
    if (tlsf == nullptr || heap == nullptr) {
        return nullptr;
    }
    auto res = tlsf_realloc(tlsf, ptr, size);
    print_memrealloc(size, ptr, res);
    return res;
}

void* reallocate_vram(void* ptr, uint32_t size) {
    if (tlsf_vram == nullptr || vram_heap == nullptr) {
        return nullptr;
    }
    auto res = tlsf_realloc(tlsf_vram, ptr, size);
    print_vmemrealloc(size, ptr, res);
    return res;
}


