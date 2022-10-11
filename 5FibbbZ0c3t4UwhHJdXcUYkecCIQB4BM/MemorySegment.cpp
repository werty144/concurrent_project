//
// Created by anton on 08.10.22.
//
#include "cstdlib"
#include "cstring"
#include <iostream>

#include "MemorySegment.hpp"
#include "Word.hpp"

MemorySegment::MemorySegment(std::size_t size, std::size_t align) {
    this->size = size;
    this->data = (char*)aligned_alloc(align, size);
    if (data == nullptr) {
        throw memory_segment_creation_exception();
    }
    memset(data,  0, size);

    size_t words_n = size / align;
    this->words = new Word[words_n];
    for (size_t i = 0; i < words_n; i++) {
        words[i].data = data + i*align;
        words[i].data_size = align;
    }
}

bool MemorySegment::contains_address(void *addr) {
    return data <= (char*) addr && (char*) addr<  data + size;
}

