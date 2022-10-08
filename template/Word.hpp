//
// Created by anton on 08.10.22.
//

#ifndef CONCURRENT_PROJECT_WORD_HPP
#define CONCURRENT_PROJECT_WORD_HPP

#include "atomic"

struct Word {
public:
    std::atomic_bool lock{false};
    std::atomic_int version{0};
    std::size_t data_size;
    void* data;
};

#endif //CONCURRENT_PROJECT_WORD_HPP
