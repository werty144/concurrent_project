#include <iostream>
#include "MemorySegment.hpp"
#include "tm.hpp"
#include <cstring>


int main() {
    MemorySegment ms = MemorySegment(4, 2);
    memset(ms.words[1].data, 1, 2);
}