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
#include "netlib/internet_address.hpp"


using namespace netlib;


#define check(cond) {\
    if (!(cond)) {\
        ++test_error_count;\
        throw test_error(__FILE__, __LINE__, #cond);\
    }\
}


#define check_enum_string(e, v)\
    check(##e##_to_string(e::v) == #v);\
    check(##e##_from_string(#v) == e::v);


#define check_exception(expr, ex)\
    do {\
        try {\
            expr;\
        }\
        catch (const ex&) {\
            break;\
        }\
        ++test_error_count;\
        throw test_error(__FILE__, __LINE__, #expr);\
    } while (0);


class test_error : public std::runtime_error {
public:
    test_error(const char* file, int line, const std::string& msg) : std::runtime_error(msg), m_file(file), m_line(line) {}

    const char* file() const { return m_file; }
    int line() const { return m_line; }

private:
    const char* m_file;
    int m_line;
};


inline size_t test_error_count = 0;


inline void init() {
    #ifdef _WIN32
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(console_handle, &mode);
    SetConsoleMode(console_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif
}


inline void cleanup() {
    std::cout << '\n';
    if (test_error_count == 0) {
        std::cout << "No errors found.\n";
    }
    else {
        std::cout << "Found " << test_error_count << (test_error_count > 1 ? " errors.\n" : " error.\n");
    }
    std::cout << '\n';
}


//get duration string
template <class T> std::string duration_string(const T& start, const T& end) {
    return " \u001b[34m[" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + " ms]\u001b[0m";
}


//execute test
template <class F> void test(const char* name, F&& proc) {
    static const auto dots = [](const std::string& s) {
        const size_t base = 50;
        const size_t count = s.size() < base ? base - s.size() : 5;
        return " \u001b[36m" + std::string(count - 2, '.') + "\u001b[0m ";
    };

    std::string title = std::string("TEST: ") + name;
    std::cout << "\u001b[36m" << "TEST: " << "\u001b[37;1m" << name << "\u001b[0m";

    const auto start = std::chrono::high_resolution_clock::now();
    
    try {
        proc();
    }
    catch (const test_error& ex) {
        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << dots(title) << "\u001b[31mERROR\u001b[0m" << duration_string(start, end) << std::endl;
        std::cout << "    " << "\u001b[33m" << "File " << ex.file() << ", line " << ex.line() << ":" << "\u001b[0m" << std::endl;
        std::cout << "    " << "\u001b[33m" << ex.what() << "\u001b[0m" << std::endl;
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
            check_enum_string(address_family, ipv4);
            check_enum_string(address_family, ipv6);
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
            check_enum_string(socket_type, stream);
            check_enum_string(socket_type, datagram);
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
            check_enum_string(protocol, tcp);
            check_enum_string(protocol, udp);
            });
    }
};


class internet_address_test {
public:
    internet_address_test() {
        test("internet_address", []() {
            //constants
            check(internet_address::ipv4_any.to_string() == "0.0.0.0");
            check(internet_address::ipv4_loopback.to_string() == "127.0.0.1");
            check(internet_address::ipv6_any.to_string() == "::");
            check(internet_address::ipv6_loopback.to_string() == "::1");

            //valid addresses/address families/hostnames
            check(internet_address("192.168.1.2", AF_INET).to_string() == "192.168.1.2");
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C", AF_INET6).to_string() == "fe80:cd00:0:cde:1257:0:211e:729c");
            check(check_host_ip(internet_address(get_host_name().c_str(), AF_INET)));
            check(check_host_ip(internet_address(get_host_name().c_str(), AF_INET6)));

            //null/empty addresses
            check(check_host_ip(internet_address(static_cast<const char*>(nullptr), AF_INET)));
            check(check_host_ip(internet_address("", AF_INET)));
            check(check_host_ip(internet_address(static_cast<const char*>(nullptr), AF_INET6)));
            check(check_host_ip(internet_address("", AF_INET6)));

            //invalid addresses/address families/host names
            check_exception(internet_address("192.I68.1.2", AF_INET), std::invalid_argument);
            check_exception(internet_address("FE80:CD00:0000:0CDE:I257:0000:211E:729C", AF_INET6), std::invalid_argument);
            check_exception(internet_address("192.168.1.2", 255), std::invalid_argument);
            check_exception(internet_address("foobar", AF_INET), std::invalid_argument);
            check_exception(internet_address("foobar", AF_INET6), std::invalid_argument);

            //autodetect addresses
            check(internet_address("192.168.1.2").to_string() == "192.168.1.2");
            check(internet_address("192.168.1.2").address_family() == AF_INET);
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C").to_string() == "fe80:cd00:0:cde:1257:0:211e:729c");
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C").address_family() == AF_INET6);
            });
    }

private:
    static std::string get_host_name() {
        char buf[256];
        gethostname(buf, sizeof(buf));
        return buf;
    }

    static void* get_sockaddr_internet_address(sockaddr* sa) {
        switch (sa->sa_family) {
        case AF_INET:
            return &reinterpret_cast<sockaddr_in*>(sa)->sin_addr;

        case AF_INET6:
            return &reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr;
        }

        return nullptr;
    }

    static bool check_host_ip(const internet_address& ia) {
        char buf[256];
        gethostname(buf, sizeof(buf));

        addrinfo* ai;
        getaddrinfo(buf, nullptr, nullptr, &ai);

        for (; ai; ai = ai->ai_next) {
            const void* a = get_sockaddr_internet_address(reinterpret_cast<sockaddr*>(ai->ai_addr));
            if (a && memcmp(ia.data(), a, ia.size()) == 0) {
                return true;
            }
        }

        return false;
    }
};


int main() {
    init();
    address_family_test();
    socket_type_test();
    protocol_test();
    internet_address_test();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
