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
#include <unordered_set>
#include "testlib.hpp"
#include "netlib/ip4_address.hpp"
#include "netlib/ip4_tcp_server_socket.hpp"
#include "netlib/ip4_tcp_client_socket.hpp"
#include "netlib/ip4_udp_socket.hpp"
#include "netlib/ip6_address.hpp"
#include "netlib/ip6_tcp_server_socket.hpp"
#include "netlib/ip6_tcp_client_socket.hpp"
#include "netlib/ip6_udp_socket.hpp"
#include "netlib/socket_poller_thread.hpp"


using namespace testlib;
using namespace netlib;


static void test_ip4_address() {
    char localhost_name[257]{};
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

    test("ip4::address::operator == ", [&]() {
        check(ip4::address("192.168.1.1") == ip4::address("192.168.1.1"));
        check(!(ip4::address("192.168.1.1") == ip4::address("192.168.1.2")));
        });

    test("ip4::address::operator != ", [&]() {
        check(ip4::address("192.168.1.1") != ip4::address("192.168.1.2"));
        check(!(ip4::address("192.168.1.1") != ip4::address("192.168.1.1")));
        });

    test("ip4::address::operator < ", [&]() {
        check(ip4::address("192.168.1.1") < ip4::address("192.168.1.2"));
        check(!(ip4::address("192.168.1.1") < ip4::address("192.168.1.1")));
        check(!(ip4::address("192.168.1.2") < ip4::address("192.168.1.1")));
        });

    test("ip4::address::operator > ", [&]() {
        check(ip4::address("192.168.1.2") > ip4::address("192.168.1.1"));
        check(!(ip4::address("192.168.1.2") > ip4::address("192.168.1.2")));
        check(!(ip4::address("192.168.1.1") > ip4::address("192.168.1.1")));
        });

    test("ip4::address::operator <= ", [&]() {
        check(ip4::address("192.168.1.1") <= ip4::address("192.168.1.1"));
        check(ip4::address("192.168.1.1") <= ip4::address("192.168.1.2"));
        check(!(ip4::address("192.168.1.2") <= ip4::address("192.168.1.1")));
        });

    test("ip4::address::operator >= ", [&]() {
        check(ip4::address("192.168.1.2") >= ip4::address("192.168.1.2"));
        check(ip4::address("192.168.1.2") >= ip4::address("192.168.1.1"));
        check(!(ip4::address("192.168.1.1") >= ip4::address("192.168.1.2")));
        });

    test("ip4::address::hash()", [&]() {
        std::unordered_set<ip4::address> set;
        set.insert(ip4::address("192.168.1.1"));
        set.insert(ip4::address("192.168.1.2"));
        set.insert(ip4::address("192.168.1.3"));
        check(set.size() == 3);
        });
};


static void test_ip4_tcp_sockets() {
    static constexpr size_t message_count = 10;
    const char* message = "hello world!!!";
    const size_t msglen = strlen(message);

    test("ip4 tcp sockets", [&]() {
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

    test("ip4 udp sockets", [&]() {
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


static void test_ip6_address() {
    char localhost_name[257]{};
    gethostname(localhost_name, sizeof(localhost_name));

    ip6::address::bytes_type localhost_address_bytes;
    ip6::address::words_type localhost_address_words;
    uint32_t localhost_zone_index;
    bool found = get_address_info(localhost_name, [&](addrinfo* ai) {
        if (ai->ai_family == AF_INET6) {
            localhost_address_bytes = reinterpret_cast<const ip6::address::bytes_type&>(reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_addr);
            localhost_address_words = reinterpret_cast<const ip6::address::words_type&>(reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_addr);
            localhost_zone_index = reinterpret_cast<sockaddr_in6*>(ai->ai_addr)->sin6_scope_id;
            return true;
        }
        return false;
        });
    check(found);

    char localhost_address_string_buffer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &localhost_address_bytes, localhost_address_string_buffer, sizeof(localhost_address_string_buffer));
    const std::string localhost_address_string = std::string(localhost_address_string_buffer) + '%' + std::to_string(localhost_zone_index);

    const std::string test_addr_string = "2001:0db8:0000:0000:0000:ff00:0042:8329%1";
    const std::string test_addr_string_abbreviated = "2001:db8::ff00:42:8329%1";
    const std::string error_test_addr_string = "2001:0db8:0000:0000:0O00:ff00:0042:8329";
    in6_addr test_addr;
    inet_pton(AF_INET6, test_addr_string.c_str(), &test_addr);
    const ip6::address::bytes_type test_addr_bytes = reinterpret_cast<const ip6::address::bytes_type&>(test_addr);
    const ip6::address::words_type test_addr_words = reinterpret_cast<const ip6::address::words_type&>(test_addr);

    test("ip6::address::address()", [&]() {
        ip6::address a;
        check(a.words() == ip6::address::words_type{});
        check(a.bytes() == ip6::address::bytes_type{});
        check(a.zone_index() == 0);
        });

    test("ip6::address::address(words_type)", [&]() {
        ip6::address a(test_addr_words, 1);
        check(a.words() == test_addr_words);
        check(a.bytes() == test_addr_bytes);
        check(a.zone_index() == 1);
        });

    test("ip6::address::address(bytes_type)", [&]() {
        ip6::address a(test_addr_bytes, 1);
        check(a.words() == test_addr_words);
        check(a.bytes() == test_addr_bytes);
        check(a.zone_index() == 1);
        });

    test("ip6::address::address(nullptr)", [&]() {
        ip6::address a(nullptr);
        check(a.words() == localhost_address_words);
        check(a.bytes() == localhost_address_bytes);
        check(a.zone_index() == localhost_zone_index);
        });

    test("ip6::address::address(\"\")", [&]() {
        ip6::address a("");
        check(a.words() == localhost_address_words);
        check(a.bytes() == localhost_address_bytes);
        check(a.zone_index() == localhost_zone_index);
        });

    test("ip6::address::address(\"" + std::string(test_addr_string) + "\")", [&]() {
        ip6::address a(test_addr_string);
        check(a.words() == test_addr_words);
        check(a.bytes() == test_addr_bytes);
        check(a.zone_index() == 1);
        });

    test("ip6::address::address(<localhost name>)", [&]() {
        ip6::address a(localhost_name);
        check(a.words() == localhost_address_words);
        check(a.bytes() == localhost_address_bytes);
        check(a.zone_index() == localhost_zone_index);
        });

    test("ip6::address::address(\"" + error_test_addr_string + "\")", [&]() {
        check_exception(ip6::address{ error_test_addr_string }, std::runtime_error);
        });

    test("ip6::address::operator = (words_type)", [&]() {
        ip6::address a;
        a = test_addr_words;
        check(a.words() == test_addr_words);
        check(a.bytes() == test_addr_bytes);
        });

    test("ip6::address::operator = (bytes_type)", [&]() {
        ip6::address a;
        a = test_addr_bytes;
        check(a.words() == test_addr_words);
        check(a.bytes() == test_addr_bytes);
        });

    test("ip6::address::operator = (nullptr)", [&]() {
        ip6::address a;
        a = nullptr;
        check(a.words() == localhost_address_words);
        check(a.bytes() == localhost_address_bytes);
        check(a.zone_index() == localhost_zone_index);
        });

    test("ip6::address::operator = (\"\")", [&]() {
        ip6::address a;
        a = "";
        check(a.words() == localhost_address_words);
        check(a.bytes() == localhost_address_bytes);
        check(a.zone_index() == localhost_zone_index);
        });

    test("ip6::address::operator = (\"" + std::string(test_addr_string) + "\")", [&]() {
        ip6::address a;
        a = test_addr_string;
        check(a.words() == test_addr_words);
        check(a.bytes() == test_addr_bytes);
        check(a.zone_index() == 1);
        });

    test("ip6::address::operator = (<localhost name>)", [&]() {
        ip6::address a;
        a = localhost_name;
        check(a.words() == localhost_address_words);
        check(a.bytes() == localhost_address_bytes);
        check(a.zone_index() == localhost_zone_index);
        });

    test("ip6::address::operator = (\"" + error_test_addr_string + "\")", [&]() {
        ip6::address a;
        check_exception(a = error_test_addr_string, std::runtime_error);
        });

    test("ip6::address()::to_string()", [&]() {
        check(ip6::address().to_string() == "::");
        });

    test("ip6::address(words_type)::to_string()", [&]() {
        check(ip6::address(test_addr_words, 1).to_string() == test_addr_string_abbreviated);
        });

    test("ip6::address(bytes_type)::to_string()", [&]() {
        check(ip6::address(test_addr_bytes, 1).to_string() == test_addr_string_abbreviated);
        });

    test("ip6::address(nullptr)::to_string()", [&]() {
        check(ip6::address(nullptr).to_string() == localhost_address_string);
        });

    test("ip6::address(\"\")::to_string()", [&]() {
        check(ip6::address("").to_string() == localhost_address_string);
        });

    test("ip6::address(\"" + std::string(test_addr_string) + "\")::to_string()", [&]() {
        check(ip6::address(test_addr_string).to_string() == test_addr_string_abbreviated);
        });

    test("ip6::address(<localhost name>)::to_string()", [&]() {
        check(ip6::address(localhost_name).to_string() == localhost_address_string);
        });

    test("ip6::address = words_type; to_string()", [&]() {
        ip6::address a;
        a = test_addr_words;
        a.set_zone_index(1);
        check(a.to_string() == test_addr_string_abbreviated);
        });

    test("ip6::address = bytes_type; to_string()", [&]() {
        ip6::address a;
        a = test_addr_bytes;
        a.set_zone_index(1);
        check(a.to_string() == test_addr_string_abbreviated);
        });

    test("ip6::address = nullptr; to_string()", [&]() {
        ip6::address a;
        a = nullptr;
        check(a.to_string() == localhost_address_string);
        });

    test("ip6::address = \"\"; to_string()", [&]() {
        ip6::address a;
        a = "";
        check(a.to_string() == localhost_address_string);
        });

    test("ip6::address = \"" + std::string(test_addr_string) + "\"; to_string()", [&]() {
        ip6::address a;
        a = test_addr_string;
        a.set_zone_index(1);
        check(a.to_string() == test_addr_string_abbreviated);
        });

    test("ip6::address = <localhost name>; to_string()", [&]() {
        ip6::address a;
        a = localhost_name;
        check(a.to_string() == localhost_address_string);
        });

    test("ip6::address::operator == ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") == ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") == ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322")));
        });

    test("ip6::address::operator != ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") != ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") != ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321")));
        });

    test("ip6::address::operator < ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") < ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") < ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321")));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322") < ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321")));
        });

    test("ip6::address::operator > ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322") > ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322") > ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322")));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") > ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321")));
        });

    test("ip6::address::operator <= ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") <= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321"));
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") <= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322") <= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321")));
        });

    test("ip6::address::operator >= ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322") >= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322"));
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322") >= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321") >= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322")));
        });

    test("ip6::address::operator == ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") == ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") == ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2")));
        });

    test("ip6::address::operator != ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2") != ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") != ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1")));
        });

    test("ip6::address::operator < ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") < ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2") < ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1")));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") < ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1")));
        });

    test("ip6::address::operator > ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2") > ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") > ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2")));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") > ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1")));
        });

    test("ip6::address::operator <= ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") <= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") <= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2") <= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1")));
        });

    test("ip6::address::operator >= ", [&]() {
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2") >= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        check(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") >= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        check(!(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1") >= ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%2")));
        });

    test("ip6::address::hash()", [&]() {
        std::unordered_set<ip6::address> set;
        set.insert(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321"));
        set.insert(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322"));
        set.insert(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8323"));
        set.insert(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8321%1"));
        set.insert(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8322%1"));
        set.insert(ip6::address("2001:0db8:0000:0000:0000:ff00:0042:8323%1"));
        check(set.size() == 6);
        });
};


static void test_ip6_tcp_sockets() {
    static constexpr size_t message_count = 10;
    const char* message = "hello world!!!";
    const size_t msglen = strlen(message);

    test("ip6 tcp sockets", [&]() {
        std::thread server_thread([&]() {
            try {
                ip6::tcp::server_socket server(ip6::socket_address(ip6::address::loopback, 10000));
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
                ip6::tcp::client_socket socket(ip6::socket_address(ip6::address::loopback, 10000));

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


static void test_ip6_udp_sockets() {
    static constexpr size_t message_count = 10;
    const char* message = "hello world!!!";
    const size_t msglen = strlen(message);
    const ip6::socket_address test_addr(ip6::address::loopback, 10000);
    ip6::udp::socket socket(test_addr);

    test("ip6 udp sockets", [&]() {
        std::thread server_thread([&]() {
            try {
                ip6::socket_address from_addr;
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


static void test_socket_poller() {   
    static constexpr size_t test_message_count = 100;
    const std::string message("hello world!");

    test("socket polling", [&]() {
        const ip4::socket_address test_addr1(ip4::address::loopback, 10000);
        const ip4::socket_address test_addr2(ip4::address::loopback, 10001);
        const ip4::socket_address test_addr3(ip4::address::loopback, 10002);

        ip4::udp::socket socket1(test_addr1);
        ip4::udp::socket socket2(test_addr2);
        ip4::udp::socket socket3(test_addr3);

        std::array<const ip4::socket_address*, 3> socket_addresses{ &test_addr1, &test_addr2, &test_addr3 };
        std::array<ip4::udp::socket*, 3> sockets{ &socket1, &socket2, &socket3 };

        std::atomic<size_t> message_counter{ 0 };

        auto callback = [&](netlib::socket_poller& poller, netlib::socket& s, socket_poller::event_type e, socket_poller::status_flags flags) {
            try {
                ip4::socket_address addr;
                static_cast<ip4::udp::socket&>(s).receive(temp_byte_buffer(), addr);
                const std::string msg(temp_byte_buffer().begin(), temp_byte_buffer().end());
                check(msg == message);
                message_counter.fetch_add(1, std::memory_order_relaxed);
            }
            catch (const std::exception&) {
                poller.remove(s);
            }
        };

        socket_poller_thread socket_poller;

        for (auto* socket : sockets) {
            socket_poller.add(*socket, callback);
        }

        //producer thread
        std::thread producer_thread([&]() {
            for (size_t i = 0; i < test_message_count; ++i) {
                sockets[i % sockets.size()]->send(temp_byte_buffer(message.begin(), message.end()), *socket_addresses[i % sockets.size()]);
            }
            });

        //wait for the producer thread to sent all its messages
        producer_thread.join();

        //wait for all messages to be received
        while (message_counter.load(std::memory_order_acquire) < test_message_count) {
            std::this_thread::yield();
        }
        });
}


int main() {
    init();
    //test_ip4_address();
    //test_ip4_tcp_sockets();
    //test_ip4_udp_sockets();
    //test_ip6_address();
    //test_ip6_tcp_sockets();
    //test_ip6_udp_sockets();
    //test_socket_poller();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
