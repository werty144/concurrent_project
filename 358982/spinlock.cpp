//
// Created by anton on 16.10.22.
//

#include <atomic>
#include "spinlock.h"

bool spinlock::lock() noexcept {
    for (;;) {
        // Optimistically assume the locked is free on the first try
        if (!lock_.exchange(true, std::memory_order_acquire)) {
            return true;
        }

        std::size_t attempts = 0;
        // Wait for locked to be released without generating cache misses
        while (lock_.load(std::memory_order_relaxed) && attempts < n_attempts) {
            // Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
            // hyper-threads
            __builtin_ia32_pause();
            attempts++;
        }
    }
}

void spinlock::unlock() noexcept {
    lock_.store(false, std::memory_order_release);
}

bool spinlock::try_lock() noexcept {
    // First do a relaxed load to check if locked is free in order to prevent
    // unnecessary cache misses if someone does while(!try_lock())
    return !lock_.load(std::memory_order_relaxed) &&
           !lock_.exchange(true, std::memory_order_acquire);
}


