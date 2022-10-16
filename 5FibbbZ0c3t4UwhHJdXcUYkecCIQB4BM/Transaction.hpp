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
    std::vector<void*> read_set{};
    std::vector<Write> write_set{};
    std::vector<void*> write_buffers{};
    void* write_buffer;
    int words_in_buffer;

    Transaction(TransactionalMemory* tm, bool is_ro);
    bool read(void const* source, std::size_t size, void* target);
    void write(void const* source, std::size_t size, void* target);
    bool end();
private:
    bool valid_read(void* word_data) const;
    void clean_up();
    bool lock_write_set();
};


#endif //CONCURRENT_PROJECT_TRANSACTION_HPP
