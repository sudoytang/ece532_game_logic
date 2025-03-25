#pragma once

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <utility>

constexpr uint32_t HEAP_SIZE = 1024 * 1024 * 32;  // 32MB

int memory_init();
void memory_end();



void* allocate(uint32_t size);
void* allocate_aligned(uint32_t size, uint32_t alignment);

void deallocate(void* ptr);

void* reallocate(void* ptr, uint32_t size);

template<typename T>
struct ManagedDeleter {
    void operator()(T* ptr) const {
        if (ptr == nullptr) {
            return;
        }
        ptr->~T();
        deallocate(ptr);
    }
};


template<typename T, typename D = ManagedDeleter<T>>
class Managed {
private:
    T* ptr;
public:
    Managed(T* ptr = nullptr) : ptr(ptr) {}
    Managed(Managed&& other) : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    Managed& operator=(Managed&& other) noexcept {
        swap(other);
        return *this;
    }

    Managed(const Managed& other) = delete;
    Managed& operator=(const Managed& other) = delete;

    ~Managed() {
    	D()(ptr);
    }
    T* get() const {
        return ptr;
    }
    T* release() {
        T* ptr = this->ptr;
        this->ptr = nullptr;
        return ptr;
    }
    void reset(T* ptr = nullptr) {
        if (ptr != this->ptr) {
            if (this->ptr != nullptr) {
                this->ptr->~T();
                deallocate(this->ptr);
            }
            this->ptr = ptr;
        }
    }
    T& operator*() const {
        return *ptr;
    }
    T* operator->() const {
        return ptr;
    }
    operator bool() const {
        return ptr != nullptr;
    }
    void swap(Managed& other) {
        std::swap(ptr, other.ptr);
    }

    Managed& operator=(std::nullptr_t) {
        reset(nullptr);
        return *this;
    }

};

template<typename T, typename DMEM>
struct ManagedArrayDeleter {
	void operator()(T* ptr, unsigned size) const {
		for (unsigned i = 0; i < size; i++) {
			ptr[i].~T();
		}
		DMEM()(ptr);
	}
};


template<typename T, typename D = ManagedArrayDeleter<T, ManagedDeleter<T>>>
class ManagedArray {
private:
    T* ptr;
    uint32_t size;
public:
    ManagedArray(T* ptr = nullptr, uint32_t size = 0) : ptr(ptr), size(size) {}
    ManagedArray(ManagedArray&& other) : ptr(other.ptr), size(other.size) {
        other.ptr = nullptr;
        other.size = 0;
    }

    ManagedArray& operator=(ManagedArray&& other) noexcept {
        swap(other);
        return *this;
    }

    ManagedArray(const ManagedArray& other) = delete;
    ManagedArray& operator=(const ManagedArray& other) = delete;

    ~ManagedArray() {
    	D()(ptr, size);
    }
    T* get() const {
        return ptr;
    }
    uint32_t get_size() const {
        return size;
    }
    std::pair<T*, uint32_t> release() {
        std::pair<T*, uint32_t> result = {ptr, size};
        ptr = nullptr;
        size = 0;
        return result;
    }
    void reset(T* ptr = nullptr, uint32_t size = 0) {
        if (ptr != this->ptr) {
            if (this->ptr != nullptr) {
                for (uint32_t i = 0; i < size; i++) {
                    this->ptr[i].~T();
                }
                deallocate(this->ptr);
            }
            this->ptr = ptr;
            this->size = size;
        }
    }
    T& operator*() const {
        return *ptr;
    }
    T& operator[](uint32_t index) const {
        return ptr[index];
    }
    T* operator->() const {
        return ptr;
    }
    operator bool() const {
        return ptr != nullptr;
    }
    void swap(ManagedArray& other) {
        std::swap(ptr, other.ptr);
        std::swap(size, other.size);
    }

    ManagedArray& operator=(std::nullptr_t) {
        reset(nullptr, 0);
        return *this;
    }
};

template<typename T, typename... Args>
Managed<T> makeManaged(Args&&... args) {
    T* ptr = (T*)allocate(sizeof(T));
    if (ptr == nullptr) {
        return nullptr;
    }
    new (ptr) T(std::forward<Args>(args)...);
    return Managed<T>(ptr);
}


template<typename T, typename... Args>
ManagedArray<T> makeManagedArray(uint32_t size, Args&&... args) {
    T* ptr = (T*)allocate(sizeof(T) * size);
    if (ptr == nullptr) {
        return nullptr;
    }
    for (uint32_t i = 0; i < size; i++) {
        new (&ptr[i]) T(std::forward<Args>(args)...);
    }
    return ManagedArray<T>(ptr, size);
}

#endif  // MEMORY_HPP
