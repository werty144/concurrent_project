#include <iostream>
#include "MemorySegment.hpp"
#include "tm.hpp"
#include "TransactionalMemory.hpp"
#include <cstring>
#include <cstddef>
#include <bitset>
#include "vector"
#include "Transaction.hpp"

using namespace std;

std::ostream& operator<< (std::ostream& os, std::byte b) {
    return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
    auto* tm = new TransactionalMemory(8, 2);
    char* local_write_buffer = (char*)malloc(4);
    memset(local_write_buffer, 42, 4);

    auto* tx = new Transaction(tm, false);
    tx->write(local_write_buffer, 4, tm->start_segment->data);

    byte* local_read_buffer = (byte*)malloc(4);
    tx->read(tm->start_segment->data, 4, local_read_buffer);
    for (size_t i = 0; i < 4; i++) {
        cout << local_read_buffer[i] << endl;
    }
}