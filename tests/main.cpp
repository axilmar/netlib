#include <cassert>
#include <vector>
#include <iostream>
#include <set>
#include <random>
#include <array>
#include <iomanip>
#include "netlib.hpp"
#include "netlib/internals/../../../src/netlib/internals/typeinfo.hpp"


using namespace netlib;


namespace typeinfo_test {
    struct s1 {
    };

    class c1 {
    };

    enum e1 {
    };

    enum class e2 {
    };

    union u1 {
    };

    using proc = void (*)();
}


static void test_typeinfo(const std::string& type, const std::string& ns, const std::string& name) {
    std::cout << "testing " << type << " -> ";
    const auto p1 = internals::split_typeinfo_name(type);
    std::cout << p1.first << "::" << p1.second << std::endl;
    assert(p1.first == ns);
    assert(p1.second == name);
}


static void test_typeinfo() {
    test_typeinfo(typeid(typeinfo_test::s1).name(), "typeinfo_test", "s1");
    test_typeinfo(typeid(typeinfo_test::c1).name(), "typeinfo_test", "c1");
    test_typeinfo(typeid(typeinfo_test::e1).name(), "typeinfo_test", "e1");
    test_typeinfo(typeid(typeinfo_test::e2).name(), "typeinfo_test", "e2");
    test_typeinfo(typeid(typeinfo_test::u1).name(), "typeinfo_test", "u1");
    test_typeinfo(typeid(typeinfo_test::proc).name(), "", "void (__cdecl*)(void)");
    test_typeinfo(typeid(int).name(), "", "int");
}


class my_object {
public:
    float value{0};
    my_object(float v = 0) : value(v) {}
};


class test_message : public message {
public:
    test_message() : message(auto_message_id<test_message>::get_message_id()) {
    }

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


static void test_message_serialization() {
    test_message msg1;

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


class test_messaging_interface : public messaging_interface {
public:

protected:
    bool send_data(byte_buffer& buffer) final {
        temp_buffer.clear();
        temp_buffer.insert(temp_buffer.end(), buffer.begin(), buffer.end());
        return true;
    }

    bool receive_data(byte_buffer& buffer) final {
        buffer = temp_buffer;
        return true;
    }
};


static void test_messaging_interface_class() {
    test_messaging_interface te;
    test_message msg1;

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


static constexpr uint16_t TCP_TEST_PORT = 10000;
static constexpr uint16_t UDP_TEST_PORT = 10000;


static void test_sockets() {
    try {
        static constexpr int COUNT = 100;
        int consumer_count = 0;

        socket_address test_addr({ "localhost", socket_address::ADDRESS_FAMILY_IP4 }, UDP_TEST_PORT);

        socket test_socket(socket::TYPE::UDP_IP4);
        test_socket.bind(test_addr);

        std::thread producer_thread([&]() {
            try {
                byte_buffer buffer;

                std::string msg("hello world!");
                for (char c : msg) {
                    buffer.push_back((byte)c);
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
                    for (byte b : buffer) {
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


class text_message : public message {
public:
    field<std::string> text;    
    text_message() : message(auto_message_id<text_message>::get_message_id()) {
    }
};


static void test_socket_messaging_interface_udp() {
    try {
        static constexpr int COUNT = 100;
        int consumer_count = 0;

        udp_messaging_interface test_socket{ constants::ADDRESS_FAMILY_IP4 };
        test_socket.bind_socket({ {"localhost", socket_address::ADDRESS_FAMILY_IP4}, UDP_TEST_PORT });
        test_socket.connect_socket({ {"localhost", socket_address::ADDRESS_FAMILY_IP4}, UDP_TEST_PORT });

        std::thread producer_thread([&]() {
            try {

                text_message tm;

                for (size_t i = 0; i < COUNT; ++i) {
                    tm.text = stringstream() << i << ": hello world!!!";
                    test_socket.send_message(tm);
                    printf("%i : send message: %s\n", consumer_count, tm.text.c_str());
                }
            }
            catch (const socket_error& err) {
                std::cout << "producer error: " << err.what() << std::endl;
            }
            });

        std::thread consumer_thread([&]() {
            try {
                for (consumer_count = 0; consumer_count < COUNT; ++consumer_count) {
                    message_pointer msg = test_socket.receive_message();
                    text_message* tm = static_cast<text_message*>(msg.get());
                    printf("%i : received message: %s\n", consumer_count, tm->text.c_str());
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


static void test_socket_messaging_interface_tcp() {
    try {
        static constexpr int COUNT = 100;
        int consumer_count = 0;

        socket_address test_addr({ "localhost", socket_address::ADDRESS_FAMILY_IP4 }, TCP_TEST_PORT);

        std::thread server_thread([&]() {
            try {
                socket server_socket{ socket::TYPE::TCP_IP4 };
                server_socket.bind(test_addr);
                server_socket.listen();
                tcp_messaging_interface test_socket{ std::move(server_socket.accept().first) };

                text_message tm;

                for (size_t i = 0; i < COUNT; ++i) {
                    tm.text = stringstream() << i << ": hello world!!!";
                    test_socket.send_message(tm);
                }
            }
            catch (const socket_error& err) {
                std::cout << "server error: " << err.what() << std::endl;
            }
            });

        std::thread client_thread([&]() {
            try {
                tcp_messaging_interface test_socket{ constants::ADDRESS_FAMILY_IP4 };
                test_socket.connect_socket(test_addr);
                for (consumer_count = 0; consumer_count < COUNT; ++consumer_count) {
                    message_pointer msg = test_socket.receive_message();
                    text_message* tm = static_cast<text_message*>(msg.get());
                    printf("%i : received message: %s\n", consumer_count, tm->text.c_str());
                }
            }
            catch (const socket_error& err) {
                std::cout << "client error: " << err.what() << std::endl;
            }
            });

        server_thread.join();
        client_thread.join();

        assert(consumer_count == COUNT);
    }
    catch (const socket_error& err) {
        std::cout << err.what() << std::endl;
    }
}


static void test_socket_messaging_interface_udp_encrypted() {
    try {
        static constexpr int COUNT = 100;
        int consumer_count = 0;

        const auto test_key = create_random_key();

        encrypted_messaging_interface<udp_messaging_interface> test_socket{ test_key, constants::ADDRESS_FAMILY_IP4 };
        test_socket.bind_socket({ {"localhost", socket_address::ADDRESS_FAMILY_IP4}, UDP_TEST_PORT });
        test_socket.connect_socket({ {"localhost", socket_address::ADDRESS_FAMILY_IP4}, UDP_TEST_PORT });

        std::thread producer_thread([&]() {
            try {
                text_message tm;

                for (size_t i = 0; i < COUNT; ++i) {
                    tm.text = stringstream() << i << ": hello world!!!";
                    test_socket.send_message(tm);
                }
            }
            catch (const socket_error& err) {
                std::cout << "producer error: " << err.what() << std::endl;
            }
            });

        std::thread consumer_thread([&]() {
            try {
                for (consumer_count = 0; consumer_count < COUNT; ++consumer_count) {
                    message_pointer msg = test_socket.receive_message();
                    text_message* tm = static_cast<text_message*>(msg.get());
                    printf("%i : received message: %s\n", consumer_count, tm->text.c_str());
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


static void test_socket_messaging_interface_tcp_encrypted() {
    try {
        static constexpr int COUNT = 100;
        int consumer_count = 0;

        socket_address test_addr({ "localhost", socket_address::ADDRESS_FAMILY_IP4 }, TCP_TEST_PORT);

        const auto test_key = create_random_key();

        std::thread server_thread([&]() {
            try {
                socket server_socket(socket::TYPE::TCP_IP4);
                server_socket.bind(test_addr);
                server_socket.listen();
                encrypted_messaging_interface<tcp_messaging_interface> test_socket{ test_key, std::move(server_socket.accept().first) };

                text_message tm;

                for (size_t i = 0; i < COUNT; ++i) {
                    tm.text = stringstream() << i << ": hello world!!!";
                    test_socket.send_message(tm);
                }
            }
            catch (const socket_error& err) {
                std::cout << "server error: " << err.what() << std::endl;
            }
            });

        std::thread client_thread([&]() {
            try {
                encrypted_messaging_interface<tcp_messaging_interface> test_socket{ test_key, constants::ADDRESS_FAMILY_IP4 };
                test_socket.connect_socket(test_addr);
                for (consumer_count = 0; consumer_count < COUNT; ++consumer_count) {
                    message_pointer msg = test_socket.receive_message();
                    text_message* tm = static_cast<text_message*>(msg.get());
                    printf("%i : received message: %s\n", consumer_count, tm->text.c_str());
                }
            }
            catch (const socket_error& err) {
                std::cout << "client error: " << err.what() << std::endl;
            }
            });

        server_thread.join();
        client_thread.join();

        assert(consumer_count == COUNT);
    }
    catch (const socket_error& err) {
        std::cout << err.what() << std::endl;
    }
}


int main() {
    test_typeinfo();
    test_serialization_traits();
    test_message_serialization();
    test_messaging_interface_class();
    test_sockets();
    test_socket_messaging_interface_udp();
    test_socket_messaging_interface_tcp();
    test_socket_messaging_interface_udp_encrypted();
    test_socket_messaging_interface_tcp_encrypted();
    system("pause");
    return 0;
}
