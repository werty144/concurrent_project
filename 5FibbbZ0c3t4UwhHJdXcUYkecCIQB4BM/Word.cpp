//
// Created by anton on 08.10.22.
//


#include "Word.hpp"
#include "log.h"

bool Word::unlocked_or_locked_by_this_thread() const {
    return !locked.load() || owner_id.load() == std::this_thread::get_id();
}

bool Word::try_lock() {
    bool expected = false;
    if (locked.compare_exchange_strong(expected, true)) {
        owner_id.store(std::this_thread::get_id());
        return true;
    }
    return false;
}

void Word::unlock() {
    if (owner_id.load() != std::this_thread::get_id()) {
        log("Unlocked by not owner!");
        exit(-1);
    }
    owner_id.store(std::thread::id());
    locked.store(false);
}

bool Word::locked_by_this_thread() const {
    return owner_id.load() == std::this_thread::get_id();
}

