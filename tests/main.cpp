#include <cassert>
#include <vector>
#include <iostream>
#include <set>
#include "netlib/message.hpp"
#include "netlib/endpoint.hpp"
#include "netlib/message_registry.hpp"
#include "netlib/socket.hpp"


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


static void test_serialization_traits() {
    assert(has_begin_end_v<std::vector<int>>);
    assert(has_begin_end_v<std::set<int>>);
    assert(!has_begin_end_v<test_message>);

    assert(has_push_back_v<std::vector<int>>);
    assert(!has_push_back_v<std::set<int>>);
    assert(!has_push_back_v<test_message>);

    assert(!has_insert_v<std::vector<int>>);
    assert(has_insert_v<std::set<int>>);
    assert(!has_insert_v<test_message>);
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

    assert(msg2.id             == msg1.id            );
    assert(msg2.data1          == msg1.data1         );
    assert(msg2.data2          == msg1.data2         );
    assert(msg2.p1             == msg1.p1            );
    assert(msg2.object1->value == msg1.object1->value);
    assert(msg2.object2->value == msg1.object2->value);
    assert(msg2.object3->value == msg1.object3->value);
}


static byte_buffer temp_buffer;


class test_endpoint : public endpoint {
public:

protected:
    void send(byte_buffer& buffer) final {
        temp_buffer = buffer;
    }

    void receive(byte_buffer& buffer) final {
        buffer = temp_buffer;
    }
};


static void test_endpoint_() {
    test_endpoint te;

    message_registration<test_message> mr(5000);

    test_message msg1;

    msg1.id = 5000;
    msg1.data1.push_back(10);
    msg1.data2.insert(20);
    msg1.p1.first = 30;
    msg1.p1.second = 40;
    msg1.object1 = new my_object(1.0f);
    msg1.object2 = std::make_unique<my_object>(2.0f);
    msg1.object3 = std::make_shared<my_object>(3.0f);

    te.send_message(msg1);

    message_pointer received_msg = te.receive_message();
    test_message& msg2 = static_cast<test_message&>(*received_msg.get());

    assert(msg2.id == msg1.id);
    assert(msg2.data1 == msg1.data1);
    assert(msg2.data2 == msg1.data2);
    assert(msg2.p1 == msg1.p1);
    assert(msg2.object1->value == msg1.object1->value);
    assert(msg2.object2->value == msg1.object2->value);
    assert(msg2.object3->value == msg1.object3->value);
}


static void test_sockets() {
    try {
        static constexpr int COUNT = 100;
        int consumer_count = 0;

        socket_address test_addr({ "localhost", socket_address::ADDRESS_FAMILY_IP4 }, 10000);
        std::cout << "test network_address: " << test_addr.get_address().to_string() << std::endl;
        socket test_socket(socket::TYPE::UDP_IP4);
        test_socket.bind(test_addr);

        std::thread producer_thread([&]() {
            try {
                byte_buffer buffer;

                std::string msg("hello world!");
                for (char c : msg) {
                    buffer.push_back((std::byte)c);
                }

                for (size_t i = 0; i < COUNT; ++i) {
                    test_socket.send(buffer, test_addr);
                }
            }
            catch (const socket_error& err) {
                std::cout << "producer error: " << err.what() << std::endl;
            }
        });

        std::thread consumer_thread([&]() {
            try {
                byte_buffer buffer(256);

                for (consumer_count = 0; consumer_count < COUNT; ++consumer_count) {
                    const size_t received_size = test_socket.receive(buffer);
                    buffer.resize(received_size);

                    std::string str;
                    for (std::byte b : buffer) {
                        str.push_back((char)b);
                    }

                    std::cout << consumer_count << ": received message: " << str << std::endl;
                }
            }
            catch (const socket_error& err) {
                std::cout << "consumer error: " << err.what() << std::endl;
            }
        });

        producer_thread.join();
        consumer_thread.join();

        assert(consumer_count == COUNT);
    }
    catch (const socket_error& err) {
        std::cout << err.what() << std::endl;
    }
}


int main() {
    //test_serialization_traits();
    //test_message_();
    //test_endpoint_();

    test_sockets();

    system("pause");
    return 0;
}
