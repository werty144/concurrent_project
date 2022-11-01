#include <iostream>
#include <bitset>
#include "versioned_lock.h"

using namespace std;

void print_int(int a) {
    std::bitset<32> x(a);
    std::cout << x << '\n';
}

void print_bool(bool b) {
    std::bitset<8> x(b);
    std::cout << x << '\n';
}

void print_lock(VersionedLock* vls) {
    cout << vls[0].get_version_locked().first << " " << vls[0].get_version_locked().second << endl;
}

int main() {
    auto* vls = new VersionedLock[1];
    vls[0].init();
    print_lock(vls);
    cout << vls[0].try_lock() << endl;
    print_lock(vls);
    vls[0].set_version(228);
    print_lock(vls);
    vls[0].unlock();
    print_lock(vls);
}