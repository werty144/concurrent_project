//
// Created by anton on 07.10.22.
//

#ifndef CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
#define CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP

#include <exception>
#include "atomic"

typedef std::exception tm_creation_exception;

class TransactionalMemory {
private:
    char* start;
    size_t size;
    size_t align;
public:
    std::atomic_int global_clock{};
    TransactionalMemory(std::size_t size, std::size_t align);
    ~TransactionalMemory();
};


#endif //CONCURRENT_PROJECT_TRANSACTIONALMEMORY_HPP
