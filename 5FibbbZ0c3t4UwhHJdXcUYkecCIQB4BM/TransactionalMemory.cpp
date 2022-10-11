//
// Created by anton on 07.10.22.
//

#include "cstdlib"
#include "cstring"

#include "TransactionalMemory.hpp"
#include "iostream"

TransactionalMemory::TransactionalMemory(std::size_t size, std::size_t align) {
    this->size = size;
    this->align = align;
    global_clock.store(0);
    try {
        this->start_segment = new MemorySegment(size, align);
    } catch (memory_segment_creation_exception& e) {
        throw tm_creation_exception();
    }
}

Word* TransactionalMemory::get_word(void* word_data) {
    // TODO update to several segments
    int diff = (char*)word_data - start_segment->data;
    int word = diff / align;
    return &start_segment->words[word];
}


