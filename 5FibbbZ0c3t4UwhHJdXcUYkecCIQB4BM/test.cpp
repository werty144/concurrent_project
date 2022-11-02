#include <iostream>
#include <bitset>
#include "versioned_lock.h"
#include <thread>
#include <unistd.h>
#include "atomic"
#include "TransactionalMemory.hpp"
#include "Transaction.hpp"

using namespace std;

static auto* trans_mem = new TransactionalMemory(16, sizeof(size_t));

void decrease_counter() {
    bool success = false;
    while (!success) {
        auto tx = new Transaction(trans_mem, false);
        size_t cur_value;
        bool read_res = tx->read(trans_mem->start_segment->data, sizeof(size_t), &cur_value);
//        if (!read_res) continue;
        size_t new_value = cur_value - 1;
        tx->write(&new_value, sizeof(size_t), trans_mem->start_segment->data);
        success = tx->end();
//        if (success && !read_res) {
//            cout << "oops" << endl;
//        }
    }
}

void job() {
    for (size_t i = 0; i < 500000; i++) {
        decrease_counter();
    }
}


int main() {
    auto init_tx = new Transaction(trans_mem, false);
    size_t init_value = 1000000;
    init_tx->write(&init_value, sizeof(size_t), trans_mem->start_segment->data);
    cout << "Initialized: " << init_tx->end() << endl;
    thread first(job);
    thread second(job);
    first.join();
    second.join();

    auto final_tx = new Transaction(trans_mem, true);
    size_t res_value;
    final_tx->read(trans_mem->start_segment->data, sizeof(size_t), &res_value);
    cout << res_value;
}