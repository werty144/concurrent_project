//
// Created by anton on 07.10.22.
//

#include "cstdlib"
#include "cstring"

#include "TransactionalMemory.hpp"
#include "iostream"

#include "log.h"

TransactionalMemory::TransactionalMemory(std::size_t size, std::size_t align) {
    this->size = size;
    this->align = align;
    global_clock.store(0);
    try {
        start_segment = new MemorySegment(size, align);
    } catch (memory_segment_creation_exception& e) {
        throw tm_creation_exception();
    }

    segments = new MemorySegment* [MAX_SEGMENTS];
    segments[0] = start_segment;
    for (std::size_t i = 1; i < MAX_SEGMENTS; i++) {
        segments[i] = nullptr;
    }
    n_segments.store(1);
}

VersionedLock* TransactionalMemory::get_versioned_lock(void* data_address) const {

    long diff = (char*)data_address - start_segment->data;
    unsigned long index = diff / align;
    return &start_segment->versioned_locks[index];
}


