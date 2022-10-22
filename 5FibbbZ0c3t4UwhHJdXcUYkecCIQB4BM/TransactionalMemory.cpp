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
        this->start_segment = new MemorySegment(size, align);
    } catch (memory_segment_creation_exception& e) {
        throw tm_creation_exception();
    }
    reference_segment = (char*)aligned_alloc(align, size);
}

Word* TransactionalMemory::get_word(void* word_data) const {
    // TODO update to several segments
    long diff = (char*)word_data - start_segment->data;
    unsigned long word = diff / align;
    return &start_segment->words[word];
}

bool TransactionalMemory::reference_read(void const*source, void *result_to_check) const {
    unsigned long word = ((char*)source - start_segment->data) / align;
    return memcmp((char*)reference_segment + align*word, result_to_check, align) == 0;
}

void TransactionalMemory::reference_write(void const* source, void *target) const {
    unsigned long word = ((char*)target - start_segment->data) / align;
    memcpy((char*)reference_segment + align*word, source, align);
}


