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
    int v = 5;
    void* p = &v;
    void* p1 = new int[5];
    void* p2 = new int[5];
    cout << p << " " << (unsigned long) p << endl;
    unsigned long correct = get_pointer_top_digits(p);
    cout << correct << endl;
    void* res = change_pointer_top_digits_to(p, 64);
    cout << res << endl;
    cout << (void*)((char*) res + 1) << endl;
    cout << (int)get_pointer_top_digits(res) << endl;
    cout << change_pointer_top_digits_to(res, correct);
}