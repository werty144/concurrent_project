//
// Created by anton on 11.10.22.
//

#include "Transaction.hpp"
#include "TransactionalMemory.hpp"
#include "cstring"
#include "cstdlib"

using namespace std::chrono_literals;


Transaction::Transaction(TransactionalMemory* tm, bool is_ro) {
    this->tm = tm;
    this->is_ro = is_ro;
    read_version = tm->global_clock.load();
    write_buffer = aligned_alloc(tm->align, 10 * tm->align);
    write_buffers.emplace_back(write_buffer);
    words_in_buffer = 0;
}

bool Transaction::read(void const* source, std::size_t size, void *target) {
    std::size_t words_n = size / tm->align;
    for (int i = 0; i < words_n; i++) {
        void* cur_source_address = (char*)source + i * tm->align;
        void* cur_target_address = (char*)target + i * tm->align;

        if (!valid_read(cur_source_address)) {
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

bool Transaction::valid_read(void* word_data) const {
    auto* word = tm->get_word(word_data);
    return word->unlocked_or_locked_by_this_thread() && word->version <= read_version;
}

void Transaction::write(void const* source, std::size_t size, void *target) {
    std::size_t n_words = size / tm->align;
    for (std::size_t i = 0; i < n_words; i++) {
        if (words_in_buffer == 10) {
            write_buffer = aligned_alloc(tm->align, 10 * tm->align);
            write_buffers.emplace_back(write_buffer);
            words_in_buffer = 0;
        }
        memcpy((char*)write_buffer + i * tm->align,
                (char*)source + i * tm->align,
               tm->align
               );
        words_in_buffer++;

        write_set.emplace_back(
                Write {
            (char*)target + i * tm->align,
            (char*)write_buffer + i * tm->align
                }
        );
    }
}

void Transaction::clean_up() {
    for (void* buffer : write_buffers) {
        free(buffer);
    }
}

bool Transaction::end() {
    if (!lock_write_set()) {
        return false;
    }
    // TODO other steps
    clean_up();
    return true;
}

bool Transaction::lock_write_set() {
    for (std::size_t i = 0; i < write_set.size(); i++) {
        auto write = write_set[i];
        auto word = tm->get_word(write.destination);
        if (word->mutex.try_lock_for(10ms)) {
            word->set_lock_info();
        } else {
            for (std::size_t j = 0; j < i; j++) {
                word->clean_lock_info();
                word->mutex.unlock();
            }
            return false;
        }
    }
    return true;
}
