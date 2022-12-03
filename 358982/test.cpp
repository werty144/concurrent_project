#include <iostream>
#include <bitset>
#include "versioned_lock.h"
#include <thread>
#include <unistd.h>
#include "atomic"
#include "TransactionalMemory.hpp"
#include "Transaction.hpp"

using namespace std;

shared_mutex my_lock;

void take_lock_shared_and_sleep() {
    bool res = my_lock.try_lock_shared();
    cout << this_thread::get_id() << " got shared lock: " << res << ". Sleeping" << endl;
    this_thread::sleep_for(std::chrono::milliseconds(2000));
    cout << this_thread::get_id() << " awake. Releasing lock" << endl;
    my_lock.unlock_shared();
}

void take_lock_shared() {
    bool res = my_lock.try_lock_shared();
    cout << this_thread::get_id() << " got shared lock: " << res << endl;
    cout << this_thread::get_id() << " Releasing lock" << endl;
    my_lock.unlock_shared();
}

void take_lock_unique_and_sleep() {
    while (!my_lock.try_lock()){}
    cout << this_thread::get_id() << " got unique lock" << endl;
    this_thread::sleep_for(std::chrono::milliseconds(2000));
    cout << this_thread::get_id() << " awake" << endl;
    my_lock.unlock();
}



int main() {
    cout << "Start" << endl;
    thread t1(take_lock_shared_and_sleep);
    thread t2(take_lock_shared);
    thread t3(take_lock_unique_and_sleep);

    t1.join();
    t2.join();
    t3.join();
    this_thread::sleep_for(std::chrono::milliseconds(2000));
}