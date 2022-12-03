//
// Created by anton on 11.10.22.
//

#ifndef CONCURRENT_PROJECT_TRANSACTION_HPP
#define CONCURRENT_PROJECT_TRANSACTION_HPP

#include "TransactionalMemory.hpp"
#include "vector"
#include "set"

struct Write {
    void* destination;
    void* data;
    uint16_t segment_index;
};

struct Read {
    void* data;
    uint16_t segment_index;
};

class Transaction {
public:
    bool is_ro;
    TransactionalMemory* tm;
    int read_version;
    int write_version;
    std::vector<Read> read_set{};
    std::vector<Write> write_set{};
    std::set<VersionedLock*> locked_set{};

    Transaction(TransactionalMemory* tm, bool is_ro);
    bool read(void const* source, std::size_t size, void* target, uint16_t segment_index);
    void write(void const* source, std::size_t size, void* target, uint16_t segment_index);
    bool end();
    void clean_up();
private:
    bool unlocked_and_old(VersionedLock* word_data) const;
    bool lock_write_set();
    void increment_and_fetch_global_clock();
    bool validate_read_set();
    void unlock_write_set();
    void write_write_set_and_unlock();
    bool read_only_read(void const* source, std::size_t size, void* target, uint16_t segment_index);
    bool read_write_read(void const* source, std::size_t size, void* target, uint16_t segment_index);
    bool end_ro();
    bool end_wr();
};


#endif //CONCURRENT_PROJECT_TRANSACTION_HPP
