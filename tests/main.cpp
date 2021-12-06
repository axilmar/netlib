#include "../src/netlib/platform.hpp"
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
#include "netlib/ip4_address.hpp"
#include "netlib/ip4_tcp_server_socket.hpp"
#include "netlib/ip4_tcp_client_socket.hpp"
#include "netlib/ip4_udp_socket.hpp"


using namespace testlib;
using namespace netlib;


static void test_ip4_address() {
    char localhost_name[256];
    gethostname(localhost_name, sizeof(localhost_name));

    ip4::address::value_type localhost_address;
    bool found = get_address_info(localhost_name, [&](addrinfo* ai) {
        if (ai->ai_family == AF_INET) {
            localhost_address = reinterpret_cast<sockaddr_in*>(ai->ai_addr)->sin_addr.S_un.S_addr;
            return true;
        }
        return false;
        });   
    check(found);

    char localhost_address_string[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &localhost_address, localhost_address_string, sizeof(localhost_address_string));

    const std::string test_addr_string = "192.168.1.2";
    const std::string error_test_addr_string = "192.168.I.2";
    in_addr test_addr;
    inet_pton(AF_INET, test_addr_string.c_str(), &test_addr);
    const std::string test_addr_param_str = std::to_string(test_addr.S_un.S_un_b.s_b1) + ", " + std::to_string(test_addr.S_un.S_un_b.s_b2) + ", " + std::to_string(test_addr.S_un.S_un_b.s_b3) + ", " + std::to_string(test_addr.S_un.S_un_b.s_b4);
    const ip4::address::bytes_type test_addr_bytes{ test_addr.S_un.S_un_b.s_b1, test_addr.S_un.S_un_b.s_b2, test_addr.S_un.S_un_b.s_b3, test_addr.S_un.S_un_b.s_b4 };

    test("ip4::address::address()", [&]() {
        ip4::address a;
        check(a.value() == 0);
        check(a.bytes() == ip4::address::bytes_type{});
        });

    test("ip4::address::address(value_type)", [&]() {
        ip4::address a(test_addr.S_un.S_addr);
        check(a.value() == test_addr.S_un.S_addr);
        check((a.bytes() == test_addr_bytes));
        });

    test("ip4::address::address(bytes_type)", [&]() {
        ip4::address a(test_addr_bytes);
        check(a.value() == test_addr.S_un.S_addr);
        check((a.bytes() == test_addr_bytes));
        });

    test("ip4::address::address(" + test_addr_param_str + ")", [&]() {
        ip4::address a(test_addr.S_un.S_un_b.s_b1, test_addr.S_un.S_un_b.s_b2, test_addr.S_un.S_un_b.s_b3, test_addr.S_un.S_un_b.s_b4);
        check(a.value() == test_addr.S_un.S_addr);
        check((a.bytes() == test_addr_bytes));
        });

    test("ip4::address::address(nullptr)", [&]() {
        ip4::address a(nullptr);
        check(a.value() == localhost_address);
        });

    test("ip4::address::address(\"\")", [&]() {
        ip4::address a("");
        check(a.value() == localhost_address);
        });

    test("ip4::address::address(\"" + std::string(test_addr_string) + "\")", [&]() {
        ip4::address a(test_addr_string);
        check(a.value() == test_addr.S_un.S_addr);
        });

    test("ip4::address::address(<localhost name>)", [&]() {
        ip4::address a(localhost_name);
        check(a.value() == localhost_address);
        });

    test("ip4::address::address(\"" + error_test_addr_string + "\")", [&]() {
        check_exception(ip4::address{ error_test_addr_string }, std::runtime_error);
        });

    test("ip4::address::operator = (value_type)", [&]() {
        ip4::address a;
        a = test_addr.S_un.S_addr;
        check(a.value() == test_addr.S_un.S_addr);
        check((a.bytes() == test_addr_bytes));
        });

    test("ip4::address::operator = (bytes_type)", [&]() {
        ip4::address a;
        a = test_addr_bytes;
        check(a.value() == test_addr.S_un.S_addr);
        check((a.bytes() == test_addr_bytes));
        });

    test("ip4::address::operator = (nullptr)", [&]() {
        ip4::address a;        
        a = nullptr;
        check(a.value() == localhost_address);
        });

    test("ip4::address::operator = (\"\")", [&]() {
        ip4::address a;
        a = "";
        check(a.value() == localhost_address);
        });

    test("ip4::address::operator = (\"" + std::string(test_addr_string) + "\")", [&]() {
        ip4::address a;
        a = test_addr_string;
        check(a.value() == test_addr.S_un.S_addr);
        });

    test("ip4::address::operator = (<localhost name>)", [&]() {
        ip4::address a;        
        a = localhost_name;
        check(a.value() == localhost_address);
        });

    test("ip4::address::operator = (\"" + error_test_addr_string + "\")", [&]() {
        ip4::address a;
        check_exception(a = error_test_addr_string, std::runtime_error);
        });

    test("ip4::address()::to_string()", [&]() {
        check(ip4::address().to_string() == "0.0.0.0");
        });

    test("ip4::address(value_type)::to_string()", [&]() {
        check(ip4::address(test_addr.S_un.S_addr).to_string() == test_addr_string);
        });

    test("ip4::address(bytes_type)::to_string()", [&]() {
        check(ip4::address(test_addr_bytes).to_string() == test_addr_string);
        });

    test("ip4::address(" + test_addr_param_str + ")::to_string()", [&]() {
        check(ip4::address(test_addr.S_un.S_un_b.s_b1, test_addr.S_un.S_un_b.s_b2, test_addr.S_un.S_un_b.s_b3, test_addr.S_un.S_un_b.s_b4).to_string() == test_addr_string);
        });

    test("ip4::address(nullptr)::to_string()", [&]() {
        check(ip4::address(nullptr).to_string() == localhost_address_string);
        });

    test("ip4::address(\"\")::to_string()", [&]() {
        check(ip4::address("").to_string() == localhost_address_string);
        });

    test("ip4::address(\"" + std::string(test_addr_string) + "\")::to_string()", [&]() {
        check(ip4::address(test_addr_string).to_string() == test_addr_string);
        });

    test("ip4::address(<localhost name>)::to_string()", [&]() {
        check(ip4::address(localhost_name).to_string() == localhost_address_string);
        });

    test("ip4::address = value_type; to_string()", [&]() {
        ip4::address a;
        a = test_addr.S_un.S_addr;
        check(a.to_string() == test_addr_string);
        });

    test("ip4::address = bytes_type; to_string()", [&]() {
        ip4::address a;
        a = test_addr_bytes;
        check(a.to_string() == test_addr_string);
        });

    test("ip4::address = nullptr; to_string()", [&]() {
        ip4::address a;
        a = nullptr;
        check(a.to_string() == localhost_address_string);
        });

    test("ip4::address = \"\"; to_string()", [&]() {
        ip4::address a;
        a = "";
        check(a.to_string() == localhost_address_string);
        });

    test("ip4::address = \"" + std::string(test_addr_string) + "\"; to_string()", [&]() {
        ip4::address a;
        a = test_addr_string;
        check(a.to_string() == test_addr_string);
        });

    test("ip4::address = <localhost name>; to_string()", [&]() {
        ip4::address a;
        a = localhost_name;
        check(a.to_string() == localhost_address_string);
        });

};


static void test_ip4_tcp_sockets() {
    static constexpr size_t message_count = 10;
    const char* message = "hello world!!!";
    const size_t msglen = strlen(message);

    test("tcp sockets", [&]() {
        std::thread server_thread([&]() {
            try {
                ip4::tcp::server_socket server(ip4::socket_address(ip4::address::loopback, 10000));
                auto client = server.accept();

                for (size_t i = 0; i < message_count; ++i) {
                    check(client.socket.receive(temp_byte_buffer(), msglen) == msglen);
                    check(strncmp(temp_byte_buffer().data(), message, msglen) == 0);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Server exception: " << ex.what() << std::endl;
            }
            });

        std::thread client_thread([&]() {
            try {
                ip4::tcp::client_socket socket(ip4::socket_address(ip4::address::loopback, 10000));

                for (size_t i = 0; i < message_count; ++i) {
                    check(socket.send(temp_byte_buffer(message, message + msglen)) == msglen);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Client exception: " << ex.what() << std::endl;
            }
            });

        client_thread.join();
        server_thread.join();
        });
}


static void test_ip4_udp_sockets() {
    static constexpr size_t message_count = 10;
    const char* message = "hello world!!!";
    const size_t msglen = strlen(message);
    const ip4::socket_address test_addr(ip4::address::loopback, 10000);
    ip4::udp::socket socket(test_addr);

    test("udp sockets", [&]() {
        std::thread server_thread([&]() {
            try {
                ip4::socket_address from_addr;
                for (size_t i = 0; i < message_count; ++i) {
                    check(socket.receive(temp_byte_buffer(), from_addr) == msglen);
                    check(temp_byte_buffer().size() == msglen);
                    check(strncmp(temp_byte_buffer().data(), message, msglen) == 0);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Server exception: " << ex.what() << std::endl;
            }
            });

        std::thread client_thread([&]() {
            try {
                for (size_t i = 0; i < message_count; ++i) {
                    check(socket.send(temp_byte_buffer(message, message + msglen), test_addr) == msglen);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Client exception: " << ex.what() << std::endl;
            }
            });

        client_thread.join();
        server_thread.join();
        });
}


int main() {
    init();
    test_ip4_address();
    test_ip4_tcp_sockets();
    test_ip4_udp_sockets();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
