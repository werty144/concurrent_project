#include <iostream>
#include <bitset>
#include "versioned_lock.h"
#include <thread>
#include <unistd.h>
#include "atomic"
#include "TransactionalMemory.hpp"
#include "Transaction.hpp"

using namespace std;

void* change_pointer_top_digits_to(void* p, uint16_t n){
    unsigned long clear_mask = 0b0000000000000000111111111111111111111111111111111111111111111111;
    return (void*)((unsigned long) p & clear_mask | ((unsigned long)n << 48));
}

uint16_t get_pointer_top_digits(void* p) {
    unsigned long mask = 0b1111111111111111000000000000000000000000000000000000000000000000;
    return ((unsigned long) p & mask) >> 48;
}


int main() {
//    MemorySegment* segment = new MemorySegment(8, 2);
//    cout << (unsigned long) segment << endl;
//    cout <<
}