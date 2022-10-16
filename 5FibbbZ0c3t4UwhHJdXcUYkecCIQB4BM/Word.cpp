//
// Created by anton on 08.10.22.
//


#include "Word.hpp"

void Word::set_lock_info() {
    locked.store(true);
    owner_id.store(std::this_thread::get_id());
}

void Word::clean_lock_info() {
    locked.store(false);
    owner_id.store(std::thread::id());
}

bool Word::unlocked_or_locked_by_this_thread() const {
    return !locked.load() || owner_id.load() == std::this_thread::get_id();
}

