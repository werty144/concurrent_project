//
// Created by anton on 08.10.22.
//

#ifndef CONCURRENT_PROJECT_MEMORYSEGMENT_HPP
#define CONCURRENT_PROJECT_MEMORYSEGMENT_HPP

#include "atomic"
#include <exception>

#include "Word.hpp"

typedef std::exception memory_segment_creation_exception;

class MemorySegment {
public:
    std::size_t size;
    char* data;
    Word* words;
    MemorySegment(std::size_t size, std::size_t align);
    bool contains_address(const void* addr) const;
};

#endif //CONCURRENT_PROJECT_MEMORYSEGMENT_HPP
