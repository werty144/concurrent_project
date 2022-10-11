#include <iostream>
#include "MemorySegment.hpp"
#include "tm.hpp"
#include "TransactionalMemory.hpp"
#include <cstring>
#include <cstddef>
#include "vector"

using namespace std;


int main() {
    vector<int> a{1, 2, 3};
    for (auto i = a.rbegin(); i != a.rend(); i++) {
        cout << *i << endl;
    }
}