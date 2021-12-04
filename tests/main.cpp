#include <vector>
#include <iostream>
#include <set>
#include <random>
#include <array>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include "testlib.hpp"


using namespace testlib;


int main() {
    init();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
