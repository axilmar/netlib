#include <vector>
#include <iostream>
#include "netlib/message.hpp"


using namespace netlib;


class test_message : public message {
public:
    field<std::vector<int>> data;

    struct {
        field<float> x;
        field<float> y;
        field<float> z;
    } position;
};


int main() {
    test_message msg1;
    msg1.id = 5000;
    msg1.data.push_back(10);

    for (field_base* f = msg1.get_first_field(); f; f = f->get_next_field()) {
        std::cout << f->get_type_info().name() << std::endl;
    }

    system("pause");
    return 0;
}
