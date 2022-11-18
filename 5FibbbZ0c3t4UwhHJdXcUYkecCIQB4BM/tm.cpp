/**
 * @file   trans_mem.c
 * @author [...]
 *
 * @section LICENSE
 *
 * [...]
 *
 * @section DESCRIPTION
 *
 * Implementation of your own transaction manager.
 * You can completely rewrite this file (and create more files) as you wish.
 * Only the interface (i.e. exported symbols and semantic) must be preserved.
**/

// Requested features
//#define _GNU_SOURCE
//#define _POSIX_C_SOURCE   200809L
//#ifdef __STDC_NO_ATOMICS__
//#error Current C11 compiler does not support atomic operations
//#endif

// External headers
#include "iostream"
#include "sstream"
// Internal headers
#include <tm.hpp>
#include "TransactionalMemory.hpp"
#include "log.h"

#include "macros.h"
#include "Transaction.hpp"

using namespace std;


/** Create (i.e. allocate + init) a new shared memory region, with one first non-free-able allocated segment of the requested size and alignment.
 * @param size  Size of the first shared segment of memory to allocate (in bytes), must be a positive multiple of the alignment
 * @param align Alignment (in bytes, must be a power of 2) that the shared memory region must support
 * @return Opaque shared memory region handle, 'invalid_shared' on failure
**/
shared_t tm_create(size_t size, size_t align) noexcept {
    TransactionalMemory* tm;
    try {
        tm = new TransactionalMemory(size, align);
    } catch (tm_creation_exception& e) {
        return invalid_shared;
    }
    return tm;
}

/** Destroy (i.e. clean-up + free) a given shared memory region.
 * @param shared Shared memory region to destroy, with no running transaction
**/
void tm_destroy(shared_t shared) noexcept {
    auto* tm = (TransactionalMemory*) shared;
//    TODO implement segments structure to keep track of non-freed segments
}

/** [thread-safe] Return the start_segment address of the first allocated segment in the shared memory region.
 * @param shared Shared memory region to query
 * @return Start address of the first allocated segment
**/
void* tm_start(shared_t shared) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    return TransactionalMemory::create_opaque_data_pointer(tm->start_segment->data, 0);
}

/** [thread-safe] Return the size (in bytes) of the first allocated segment of the shared memory region.
 * @param shared Shared memory region to query
 * @return First allocated segment size
**/
size_t tm_size(shared_t shared) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    return tm->size;
}

/** [thread-safe] Return the alignment (in bytes) of the memory accesses on the given shared memory region.
 * @param shared Shared memory region to query
 * @return Alignment used globally
**/
size_t tm_align(shared_t shared) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    return tm->align;
}

/** [thread-safe] Begin a new transaction on the given shared memory region.
 * @param shared Shared memory region to start_segment a transaction on
 * @param is_ro  Whether the transaction is read-only
 * @return Opaque transaction ID, 'invalid_tx' on failure
**/
tx_t tm_begin(shared_t shared, bool is_ro) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    auto* transaction = new Transaction(tm, is_ro);
    return (tx_t) transaction;
}

/** [thread-safe] End the given transaction.
 * @param shared Shared memory region associated with the transaction
 * @param tx     Transaction to end
 * @return Whether the whole transaction committed
**/
bool tm_end(shared_t shared, tx_t tx) noexcept {
    auto* tm  = (TransactionalMemory*) shared;
    auto* transaction = (Transaction*) tx;
    bool success = transaction->end();
    transaction->clean_up();
    return success;
}

/** [thread-safe] Read operation in the given transaction, source in the shared region and target in a private region.
 * @param shared Shared memory region associated with the transaction
 * @param tx     Transaction to use
 * @param source Source start_segment address (in the shared region)
 * @param size   Length to copy (in bytes), must be a positive multiple of the alignment
 * @param target Target start_segment address (in a private region)
 * @return Whether the whole transaction can continue
**/
bool tm_read(shared_t shared, tx_t tx, void const* source, size_t size, void* target) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    transparent_data_pointer p = tm->create_transparent_data_pointer(source);
    uint16_t segment_index = TransactionalMemory::get_pointer_top_digits(source);
    auto* transaction = (Transaction*) tx;
    bool success = transaction->read(p, size, target, segment_index);
    if (!success) {
        transaction->clean_up();
    }
    return success;
}

/** [thread-safe] Write operation in the given transaction, source in a private region and target in the shared region.
 * @param shared Shared memory region associated with the transaction
 * @param tx     Transaction to use
 * @param source Source start_segment address (in a private region)
 * @param size   Length to copy (in bytes), must be a positive multiple of the alignment
 * @param target Target start_segment address (in the shared region)
 * @return Whether the whole transaction can continue
**/
bool tm_write(shared_t unused(shared), tx_t tx, void const* source, size_t size, void* target) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    transparent_data_pointer p = tm->create_transparent_data_pointer(target);
    uint16_t segment_index = TransactionalMemory::get_pointer_top_digits(target);
    auto* transaction = (Transaction*)tx;
    transaction->write(source, size, p, segment_index);
    return true;
}

/** [thread-safe] Memory allocation in the given transaction.
 * @param shared Shared memory region associated with the transaction
 * @param tx     Transaction to use
 * @param size   Allocation requested size (in bytes), must be a positive multiple of the alignment
 * @param target Pointer in private memory receiving the address of the first byte of the newly allocated, aligned segment
 * @return Whether the whole transaction can continue (success/nomem), or not (abort_alloc)
**/
Alloc tm_alloc(shared_t shared, tx_t unused(tx), size_t size, void** target) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    uint16_t new_index = atomic_fetch_add(&tm->n_segments,1);
    log(to_string(new_index) + "allocating");
    auto* segment = new MemorySegment(size, tm->align);
    tm->segments[new_index] = segment;
    *target = TransactionalMemory::create_opaque_data_pointer(segment->data, new_index);
    return Alloc::success;
}

/** [thread-safe] Memory freeing in the given transaction.
 * @param shared Shared memory region associated with the transaction
 * @param tx     Transaction to use
 * @param target Address of the first byte of the previously allocated segment to deallocate
 * @return Whether the whole transaction can continue
**/
bool tm_free(shared_t shared, tx_t unused(tx), void* target) noexcept {
    auto* tm = (TransactionalMemory*) shared;
    bool expected = false;
    if (!tm->global_lock.compare_exchange_strong(expected, true)) {
        return false;
    }
    while (tm->transactions_running.load() > 1){
//        log(to_string(tm->transactions_running.load()));
    }
//    log("Out");
    uint16_t segment_index = TransactionalMemory::get_pointer_top_digits(target);
    log(to_string(segment_index));
    MemorySegment* segment = tm->segments[segment_index];
    delete[] segment->versioned_locks;
    free(segment->data);
    tm->global_lock.store(false);
    return true;
}
