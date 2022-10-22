//
// Created by anton on 20.10.22.
//

#include <thread>
#include "log.h"
#include "iostream"


void log(const std::string& message) {
    std::stringstream stream;
    stream << std::this_thread::get_id() << ": " << message << std::endl;
    std::cout << stream.str();
}