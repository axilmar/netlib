#include "../src/netlib/system.hpp"
#include <vector>
#include <iostream>
#include <set>
#include <random>
#include <array>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include "netlib/address_family.hpp"
#include "netlib/socket_type.hpp"
#include "netlib/protocol.hpp"


using namespace netlib;


static void init() {
    #ifdef _WIN32
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(console_handle, &mode);
    SetConsoleMode(console_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif
}


#define check(cond) {\
    if (!(cond)) {\
        throw std::runtime_error(#cond);\
    }\
}


//get duration string
template <class T>
static std::string duration_string(const T& start, const T& end) {
    return " \u001b[34m[" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(start - end).count()) + " ms]\u001b[0m";
}


//execute test
template <class F> static void test(const char* name, F&& proc) {
    static const auto dots = [](const std::string& s) {
        const size_t base = 30;
        const size_t count = s.size() < base ? base - s.size() : 5;
        return " \u001b[36m" + std::string(count - 2, '.') + "\u001b[0m ";
    };

    std::string title = std::string("TEST: ") + name;
    std::cout << "\u001b[36m" << "TEST: " << "\u001b[37;1m" << name << "\u001b[0m";

    const auto start = std::chrono::high_resolution_clock::now();
    
    try {
        proc();
    }
    catch (const std::exception& ex) {
        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << dots(title) << "\u001b[31mERROR\u001b[0m" << duration_string(start, end) << ' ' << "\u001b[33m" << ex.what() << "\u001b[0m" << std::endl;
        return;
    }
    
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << dots(title) << "\u001b[32mOK\u001b[0m" << duration_string(start, end) << std::endl;
}


class address_family_test {
public:
    address_family_test() {
        test("address_family", []() {
            check(address_family_to_system_value(address_family::ipv4) == AF_INET);
            check(address_family_to_system_value(address_family::ipv6) == AF_INET6);
            check(system_value_to_address_family(AF_INET) == address_family::ipv4);
            check(system_value_to_address_family(AF_INET6) == address_family::ipv6);
            });
    }
};


class socket_type_test {
public:
    socket_type_test() {
        test("socket_type", []() {
            check(socket_type_to_system_value(socket_type::stream) == SOCK_STREAM);
            check(socket_type_to_system_value(socket_type::datagram) == SOCK_DGRAM);
            check(system_value_to_socket_type(SOCK_STREAM) == socket_type::stream);
            check(system_value_to_socket_type(SOCK_DGRAM) == socket_type::datagram);
            });
    }
};


class protocol_test {
public:
    protocol_test() {
        test("protocol", []() {
            check(protocol_to_system_value(protocol::tcp) == IPPROTO_TCP);
            check(protocol_to_system_value(protocol::udp) == IPPROTO_UDP);
            check(system_value_to_protocol(IPPROTO_TCP) == protocol::tcp);
            check(system_value_to_protocol(IPPROTO_UDP) == protocol::udp);
            });
    }
};


class internet_address_test {
public:
    internet_address_test() {
    }
};


int main() {
    init();
    address_family_test();
    socket_type_test();
    protocol_test();
    internet_address_test();
    system("pause");
    return 0;
}
