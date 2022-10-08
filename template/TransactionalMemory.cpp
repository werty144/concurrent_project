//
// Created by anton on 07.10.22.
//

#include "cstdlib"
#include "cstring"

#include "TransactionalMemory.hpp"

TransactionalMemory::TransactionalMemory(std::size_t size, std::size_t align) {
    this->size = size;
    this->align = align;
    global_clock.store(0);

    start = (char*)aligned_alloc(align, size);
    if (start == nullptr) {
        throw tm_creation_exception();
    }
    memset(start,  0, size);
}

TransactionalMemory::~TransactionalMemory() {
    free(start);

}

