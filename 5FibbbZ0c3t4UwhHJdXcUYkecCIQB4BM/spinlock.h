//
// Created by anton on 16.10.22.
//

#ifndef CONCURRENT_PROJECT_SPINLOCK_H
#define CONCURRENT_PROJECT_SPINLOCK_H

struct spinlock {
    std::atomic<bool> lock_ = {false};
    std::size_t n_attempts = 100000;

    bool lock() noexcept;
    void unlock() noexcept;
    bool try_lock() noexcept;
};

#endif //CONCURRENT_PROJECT_SPINLOCK_H
