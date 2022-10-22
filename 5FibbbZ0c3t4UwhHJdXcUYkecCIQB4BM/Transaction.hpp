//
// Created by anton on 11.10.22.
//

#ifndef CONCURRENT_PROJECT_TRANSACTION_HPP
#define CONCURRENT_PROJECT_TRANSACTION_HPP

#include "TransactionalMemory.hpp"
#include "vector"

struct Write {
    void* destination;
    void* data;
};

class Transaction {
public:
    bool is_ro;
    TransactionalMemory* tm;
    int read_version;
    int write_version;
    std::vector<void*> read_set{};
    std::vector<Write> write_set{};

    Transaction(TransactionalMemory* tm, bool is_ro);
    bool read(void const* source, std::size_t size, void* target);
    void write(void const* source, std::size_t size, void* target);
    bool end();
    void clean_up();
private:
    bool valid_read(void* word_data) const;
    bool lock_write_set();
    void increment_and_fetch_global_clock();
    bool validate_read_set();
    void unlock_write_set();
    void write_write_set_and_unlock();
    bool read_only_read(void const* source, std::size_t size, void* target);
    bool read_write_read(void const* source, std::size_t size, void* target);
};


#endif //CONCURRENT_PROJECT_TRANSACTION_HPP
