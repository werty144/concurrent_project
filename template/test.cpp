#include <iostream>
#include "MemorySegment.hpp"
#include "tm.hpp"
#include "TransactionalMemory.hpp"
#include <cstring>
#include <cstddef>


int main() {
    auto* tm = new TransactionalMemory(4, 2);
    Word* start = (Word*)tm_start(tm);
    memset(start[1].data, 2, 2);

}