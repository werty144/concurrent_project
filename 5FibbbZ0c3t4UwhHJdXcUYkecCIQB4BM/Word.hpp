//
// Created by anton on 08.10.22.
//

#ifndef CONCURRENT_PROJECT_WORD_HPP
#define CONCURRENT_PROJECT_WORD_HPP

#include "atomic"
#include "thread"
#include "mutex"

struct Word {
public:
    std::timed_mutex mutex;
    std::atomic_bool locked{false};
    std::atomic<std::thread::id> owner_id{};
    std::atomic_int version{0};
    std::size_t data_size;
    void* data;

    void set_lock_info();
    void clean_lock_info();
    bool unlocked_or_locked_by_this_thread() const;
};

#endif //CONCURRENT_PROJECT_WORD_HPP
