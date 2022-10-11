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
    std::vector<Write> write_set{}; // pointer from, pointer to
    Transaction(TransactionalMemory* tm, bool is_ro);
    bool read(void* source, std::size_t size, void* target);
    bool validate_can_read(void* word_data);
};


#endif //CONCURRENT_PROJECT_TRANSACTION_HPP
