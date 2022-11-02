//
// Created by anton on 11.10.22.
//

#include "Transaction.hpp"
#include "TransactionalMemory.hpp"
#include "cstring"
#include "cstdlib"
#include "algorithm"
#include "sstream"
#include "iostream"
#include "string"
#include "log.h"


Transaction::Transaction(TransactionalMemory* tm, bool is_ro) {
    this->tm = tm;
    this->is_ro = is_ro;
    read_version = tm->global_clock.load();
    write_version = -1;
}

bool Transaction::read(void const* source, std::size_t size, void *target) {
   if (is_ro) {
       return read_only_read(source, size, target);
   } else {
       return read_write_read(source, size, target);
   }
}

bool Transaction::unlocked_and_old(VersionedLock* versioned_lock) const {
    auto version_locked = versioned_lock->get_version_locked();
    return !version_locked.second && version_locked.first <= read_version;
}

void Transaction::write(void const* source, std::size_t size, void *target) {
    std::size_t n_words = size / tm->align;
    for (std::size_t i = 0; i < n_words; i++) {
        void* new_word = malloc(tm->align);
        memcpy(new_word,
                (char*)source + i * tm->align,
               tm->align
               );

        write_set.emplace_back(
                Write {
            (char*)target + i * tm->align,
            new_word
                }
        );
    }
}

void Transaction::clean_up() {
    for (auto write : write_set) {
        free(write.data);
    }
}

bool Transaction::end() {
    if (is_ro) {
        return end_ro();
    } else {
        return end_wr();
    }
}

bool Transaction::lock_write_set() {
    for (auto write : write_set) {
        auto versioned_lock = tm->get_versioned_lock(write.destination);
        if (locked_set.find(versioned_lock) != locked_set.end()) {
            continue;
        }
        if (!versioned_lock->try_lock()) {
            unlock_write_set();
            return false;
        } else {
            locked_set.insert(versioned_lock);
        }
    }
    return true;
}

void Transaction::increment_and_fetch_global_clock() {
    write_version = atomic_fetch_add(&tm->global_clock, 1) + 1;
}

bool Transaction::validate_read_set() {
    if (write_version == read_version + 1) {
        return true;
    }

    return std::all_of(read_set.begin(), read_set.end(), [this](void* word_pointer)
    {
        auto versioned_lock = tm->get_versioned_lock(word_pointer);
        auto version_locked = versioned_lock->get_version_locked();
        if (version_locked.first > read_version) {
            return false;
        }
        if (locked_set.find(versioned_lock) != locked_set.end()) {
            return true;
        }
        return !version_locked.second;
    });
}

void Transaction::unlock_write_set() {
    for (auto write : write_set) {
        auto versioned_lock = tm->get_versioned_lock(write.destination);
        if (locked_set.find(versioned_lock) != locked_set.end()) {
            versioned_lock->unlock();
            locked_set.erase(versioned_lock);
        }
    }
}

void Transaction::write_write_set_and_unlock() {
    for (auto write : write_set) {
        auto versioned_lock = tm->get_versioned_lock(write.destination);
        memcpy(write.destination, write.data, tm->align);
        versioned_lock->set_version(write_version);
    }
    unlock_write_set();
}

bool Transaction::read_only_read(const void *source, std::size_t size, void *target) {
    std::size_t words_n = size / tm->align;
    for (int i = 0; i < words_n; i++) {
        void *cur_source_address = (char *) source + i * tm->align;
        void *cur_target_address = (char *) target + i * tm->align;
        memcpy(cur_target_address, cur_source_address, tm->align);
        VersionedLock* versioned_lock = tm->get_versioned_lock(cur_source_address);
        if (!unlocked_and_old(versioned_lock)) {
            return false;
        }
    }
    return true;
}

bool Transaction::read_write_read(const void *source, std::size_t size, void *target) {
    std::size_t words_n = size / tm->align;
    for (int i = 0; i < words_n; i++) {
        void* cur_source_address = (char*)source + i * tm->align;
        void* cur_target_address = (char*)target + i * tm->align;

        VersionedLock* versioned_lock = tm->get_versioned_lock(cur_source_address);
        if (!unlocked_and_old(versioned_lock)) {
            return false;
        }

        read_set.emplace_back(cur_source_address);

        bool found_in_write_set = false;
        for (auto write = write_set.rbegin(); write != write_set.rend(); write++) {
            if (write->destination == cur_source_address) {
                memcpy(cur_target_address, write->data, tm->align);
                found_in_write_set = true;
                break;
            }
        }

        if (!found_in_write_set) {
            memcpy(cur_target_address, cur_source_address, tm->align);
        }

    }
    return true;
}

bool Transaction::end_ro() {
    return true;
}

bool Transaction::end_wr() {
    if (!lock_write_set()) {
        return false;
    }

    increment_and_fetch_global_clock();

    if (!validate_read_set()) {
        unlock_write_set();
        return false;
    }

    write_write_set_and_unlock();

    return true;
}

bool Transaction::write_set_locked() {
    return std::all_of(write_set.begin(), write_set.end(), [this](Write write)
    {
        auto versioned_lock = tm->get_versioned_lock(write.destination);
        return versioned_lock->get_version_locked().second;
    });
}
