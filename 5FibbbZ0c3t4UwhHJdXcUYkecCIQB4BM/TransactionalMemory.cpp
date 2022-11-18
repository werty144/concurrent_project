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

    real_segment_address_prefix = get_pointer_top_digits(start_segment);
}

VersionedLock* TransactionalMemory::get_versioned_lock(transparent_data_pointer p, uint16_t segment_index) const {
    MemorySegment* segment = segments[segment_index];
    long diff = (char*)p - segment->data;
    unsigned long index = diff / align;
    return &segment->versioned_locks[index];
}

void *TransactionalMemory::create_opaque_data_pointer(transparent_data_pointer p, uint16_t segment_index) {
    return change_pointer_top_digits_to(p, segment_index);
}

void *TransactionalMemory::create_transparent_data_pointer(void const* p) const {
    return change_pointer_top_digits_to(p, real_segment_address_prefix);
}

uint16_t TransactionalMemory::get_pointer_top_digits(void const* p) {
    unsigned long mask = 0b1111111111111111000000000000000000000000000000000000000000000000;
    return ((unsigned long) p & mask) >> 48;
}

void *TransactionalMemory::change_pointer_top_digits_to(void const *p, uint16_t n) {
    unsigned long clear_mask = 0b0000000000000000111111111111111111111111111111111111111111111111;
    return (void*)((unsigned long) p & clear_mask | ((unsigned long)n << 48));
}
