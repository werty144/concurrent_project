//
// Created by anton on 11.10.22.
//

#include "Transaction.hpp"
#include "TransactionalMemory.hpp"
#include "cstring"

Transaction::Transaction(TransactionalMemory* tm, bool is_ro) {
    this->tm = tm;
    this->is_ro = is_ro;
    read_version = tm->global_clock.load();
}

bool Transaction::read(void *source, std::size_t size, void *target) {
    std::size_t words_n = size / tm->align;
    for (int i = 0; i < words_n; i++) {
        void* cur_source_address = (char*)source + i * tm->align;
        void* cur_target_address = (char*)target + i * tm->align;

        if (!validate_can_read(cur_source_address)) {
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

bool Transaction::validate_can_read(void* word_data) {
    auto* word = tm->get_word(word_data);
    return !word->lock.load() && word->version <= read_version;
}
