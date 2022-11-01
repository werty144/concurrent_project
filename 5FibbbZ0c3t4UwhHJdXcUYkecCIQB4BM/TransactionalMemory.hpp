//
// Created by anton on 07.10.22.
//

#ifndef CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
#define CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP

#include <exception>
#include "atomic"
#include "MemorySegment.hpp"

typedef std::exception tm_creation_exception;

class TransactionalMemory {
public:
    MemorySegment* start_segment;
    size_t size;
    size_t align;
    std::atomic_int global_clock{0};
    char* reference_segment;
    int total_txs = 0;
    int failed_tx = 0;
    TransactionalMemory(std::size_t size, std::size_t align);
    ~TransactionalMemory();
    VersionedLock* get_versioned_lock(void* data_address) const;
    bool reference_read(void const* source, void* result_to_check) const;
    void reference_write(void const* source, void* target) const;
};


#endif //CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
