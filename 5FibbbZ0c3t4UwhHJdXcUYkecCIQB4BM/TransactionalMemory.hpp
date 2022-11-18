//
// Created by anton on 07.10.22.
//

#ifndef CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
#define CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP

#include <exception>
#include "atomic"
#include <vector>

#include "MemorySegment.hpp"

typedef std::exception tm_creation_exception;

class TransactionalMemory {
public:
    MemorySegment* start_segment;
    size_t size;
    size_t align;
    std::atomic_int global_clock{0};
    std::atomic_int transactions_running{0};
    std::atomic_bool global_lock{false};
    const std::size_t MAX_SEGMENTS = 1000;
    std::atomic_size_t n_segments{};
    MemorySegment** segments;
    TransactionalMemory(std::size_t size, std::size_t align);
    ~TransactionalMemory();
    VersionedLock* get_versioned_lock(void* data_address) const;
};


#endif //CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
