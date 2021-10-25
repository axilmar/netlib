#include <vector>
#include <iostream>
#include <set>
#include "netlib/message.hpp"
#include "netlib/endpoint.hpp"
#include "netlib/message_registry.hpp"


using namespace netlib;


class my_object {
public:
    float value{0};
    my_object(float v = 0) : value(v) {}
};


class test_message : public message {
public:
    field<std::vector<int>> data1;
    field<std::set<int>> data2;
    field<std::pair<int, int>> p1;
    field<std::tuple<float, float, float>> p2;

    struct {
        field<float> x;
        field<float> y;
        field<float> z;
    } position;

    field<my_object*> object1{ nullptr };
    field<std::unique_ptr<my_object>> object2;
    field<std::shared_ptr<my_object>> object3;
};


static byte_buffer temp_buffer;


class test_endpoint : public endpoint {
public:

protected:
    void send(const byte_buffer& buffer) final {
        temp_buffer = buffer;
    }

    void receive(byte_buffer& buffer) final {
        buffer = temp_buffer;
    }
};


static void test_serialization_traits() {
    std::cout << "has_begin_end\n";
    std::cout << has_begin_end_v<std::vector<int>> << std::endl;
    std::cout << has_begin_end_v<std::set<int>> << std::endl;
    std::cout << has_begin_end_v<test_message> << std::endl;

    std::cout << std::endl;
    std::cout << "has_push_back\n";
    std::cout << has_push_back_v<std::vector<int>> << std::endl;
    std::cout << has_push_back_v<std::set<int>> << std::endl;
    std::cout << has_push_back_v<test_message> << std::endl;

    std::cout << std::endl;
    std::cout << "has_insert\n";
    std::cout << has_insert_v<std::vector<int>> << std::endl;
    std::cout << has_insert_v<std::set<int>> << std::endl;
    std::cout << has_insert_v<test_message> << std::endl;
}


static void test_message_() {
    test_message msg1;
    msg1.id = 5000;
    msg1.data1.push_back(10);
    msg1.data2.insert(20);
    msg1.p1.first = 30;
    msg1.p1.second = 40;
    msg1.object1 = new my_object(1.0f);
    msg1.object2 = std::make_unique<my_object>(2.0f);
    msg1.object3 = std::make_shared<my_object>(3.0f);

    byte_buffer buffer;
    msg1.serialize(buffer);

    test_message msg2;
    msg2.deserialize(buffer);
}


static void test_encryption() {
    test_endpoint te;

    message_registration<test_message> mr(5000);

    test_message msg1;

    msg1.id = 5000;
    msg1.data1.push_back(10);
    msg1.data2.insert(20);
    msg1.position.x = 100.0f;
    msg1.position.y = 200.0f;
    msg1.position.z = 300.0f;

    te.send_message(msg1);

    message_pointer msg2 = te.receive_message();

    int x = 0;
}


int main() {
    //test_serialization_traits();
    //test_message_();
    test_encryption();

    system("pause");
    return 0;
}
