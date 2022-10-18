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

using namespace std::chrono_literals;

void log(const std::string& message) {
    std::stringstream stream;
    stream << std::this_thread::get_id() << ": " << message << std::endl;
    std::cout << stream.str();
}

Transaction::Transaction(TransactionalMemory* tm, bool is_ro) {
    this->tm = tm;
    this->is_ro = is_ro;
    read_version = tm->global_clock.load();
    write_version = -1;
    write_buffer = aligned_alloc(tm->align, 10 * tm->align);
    write_buffers.emplace_back(write_buffer);
    words_in_buffer = 0;
}

bool Transaction::read(void const* source, std::size_t size, void *target) {
   if (is_ro) {
       return read_only_read(source, size, target);
   } else {
       return read_write_read(source, size, target);
   }
}

bool Transaction::valid_read(void* word_data) const {
    auto* word = tm->get_word(word_data);
    return word->unlocked_or_locked_by_this_thread() && word->version <= read_version;
}

void Transaction::write(void const* source, std::size_t size, void *target) {
    writes_n++;
    std::stringstream s;
    s << writes_n << " this: " << this;
    log(s.str());
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
    std::stringstream s;
    s << "clean " << this;
    log(s.str());
    for (void* buffer : write_buffers) {
        free(buffer);
    }
    log("clean successful");
}

bool Transaction::end() {
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

bool Transaction::lock_write_set() {
    for (std::size_t i = 0; i < write_set.size(); i++) {
        auto write = write_set[i];
        auto word = tm->get_word(write.destination);
        if (!word->try_lock()) {
            for (std::size_t j = 0; j < i; j++) {
                word->unlock();
            }
            return false;
        }
    }
    return true;
}

int Transaction::increment_and_fetch_global_clock() {
    write_version = atomic_fetch_add(&tm->global_clock, 1) + 1;
}

bool Transaction::validate_read_set() {
    if (write_version == read_version + 1) {
        return true;
    }

    return std::all_of(read_set.begin(), read_set.end(), [this](void* word_pointer)
    {
        auto word = tm->get_word(word_pointer);
        return valid_read(word->data);
    });
}

void Transaction::unlock_write_set() {
    for (auto write : write_set) {
        auto word = tm->get_word(write.destination);
        word->unlock();
    }
}

void Transaction::write_write_set_and_unlock() {
    for (auto write : write_set) {
        auto word = tm->get_word(write.destination);
        memcpy(write.destination, write.data, tm->align);
        word->version.store(write_version);
        word->unlock();
    }
}

bool Transaction::read_only_read(const void *source, std::size_t size, void *target) {
    std::size_t words_n = size / tm->align;
    for (int i = 0; i < words_n; i++) {
        void *cur_source_address = (char *) source + i * tm->align;
        void *cur_target_address = (char *) target + i * tm->align;
        memcpy(cur_target_address, cur_source_address, tm->align);
        if (!valid_read(cur_source_address)) {
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
