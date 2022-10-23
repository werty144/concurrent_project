//
// Created by anton on 08.10.22.
//

#ifndef CONCURRENT_PROJECT_WORD_HPP
#define CONCURRENT_PROJECT_WORD_HPP

#include "atomic"
#include "thread"
#include "mutex"
#include "chrono"

struct Word {
public:
    std::atomic_bool locked{false};
    std::atomic<std::thread::id> owner_id{};
    std::atomic_int version{0};
    std::size_t data_size;
    void* data;

    bool try_lock();
    void unlock();
    bool locked_by_this_thread() const;
    bool unlocked_or_locked_by_this_thread() const;
};

#endif //CONCURRENT_PROJECT_WORD_HPP
