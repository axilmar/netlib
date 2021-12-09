#include "testlib.hpp"


using namespace testlib;


int main() {
    init();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
