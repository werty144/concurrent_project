//
// Created by anton on 07.10.22.
//

#ifndef CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
#define CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP

#include <exception>
#include "atomic"
#include <mutex>
#include <shared_mutex>

#include <vector>
#include "MemorySegment.hpp"

typedef std::shared_mutex Lock;
typedef std::exception tm_creation_exception;
typedef void* opaque_data_pointer;
typedef void* transparent_data_pointer;

struct SegmentStatus {
public:
    static const std::size_t ABSENT = 0;
    static const std::size_t IN_USE = 1;
    static const std::size_t TO_BE_DELETED = 2;
    static const std::size_t DELETED = 3;
};

class TransactionalMemory {
public:
    MemorySegment* start_segment;
    size_t size;
    size_t align;
    std::atomic_int global_clock{0};
    Lock freeing_lock;
    Lock allocating_lock;
    std::atomic_int transactions_committed_since_last_free{0};
    const std::size_t CLEANING_PERIOD = 100000;
    std::size_t* segment_status;
    std::size_t MAX_SEGMENTS = 1000;
    std::atomic_uint16_t n_segments{};
    MemorySegment** segments;
    uint16_t real_segment_address_prefix;

    TransactionalMemory(std::size_t size, std::size_t align);
    ~TransactionalMemory();
    VersionedLock* get_versioned_lock(opaque_data_pointer p, uint16_t segment_index) const;

    static void* create_opaque_data_pointer(transparent_data_pointer p, uint16_t segment_index);
    void* create_transparent_data_pointer(void const* p) const;
    static uint16_t get_pointer_top_digits(void const* p);
    static void* change_pointer_top_digits_to(void const* p, uint16_t n);
    void free_marked_segments_if_time();
    void realloc_segments(std::uint16_t);
};


#endif //CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
