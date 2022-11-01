//
// Created by anton on 08.10.22.
//
#include "cstdlib"
#include "cstring"
#include <iostream>

#include "MemorySegment.hpp"
#include "log.h"

MemorySegment::MemorySegment(std::size_t size, std::size_t align) {
    this->size = size;
    this->data = (char*)aligned_alloc(align, size);
    if (data == nullptr) {
        throw memory_segment_creation_exception();
    }
    memset(data,  0, size);

    size_t locks_n = size / align;
    this->versioned_locks = new VersionedLock[locks_n];
    for (size_t i = 0; i < locks_n; i++) {
        versioned_locks[i].init();
    }
}

bool MemorySegment::contains_address(const void *addr) const {
    return data <= (char*) addr && (char*) addr <  data + size;
}
