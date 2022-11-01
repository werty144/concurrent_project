//
// Created by anton on 08.10.22.
//

#ifndef CONCURRENT_PROJECT_MEMORYSEGMENT_HPP
#define CONCURRENT_PROJECT_MEMORYSEGMENT_HPP

#include "atomic"
#include <exception>

#include "versioned_lock.h"

typedef std::exception memory_segment_creation_exception;

class MemorySegment {
public:
    std::size_t size;
    char* data;
    VersionedLock* versioned_locks;
    MemorySegment(std::size_t size, std::size_t align);
    bool contains_address(const void* addr) const;
};

#endif //CONCURRENT_PROJECT_MEMORYSEGMENT_HPP
