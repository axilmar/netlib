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

    test("ip4::address::address()", []() {
        ip4::address a;
        check(a.value() == 0);
        check(a.bytes() == ip4::address::bytes_type{});
        });

    test("ip4::address::address(value_type)", []() {
        ip4::address a(htonl(0xc0a80102));
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4::address::bytes_type{192, 168, 1, 2}));
        });

    test("ip4::address::address(bytes_type)", []() {
        ip4::address a(ip4::address::bytes_type{ 192, 168, 1, 2 });
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4::address::bytes_type{ 192, 168, 1, 2 }));
        });

    test("ip4::address::address(192, 168, 1, 2)", []() {
        ip4::address a(192, 168, 1, 2);
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4::address::bytes_type{ 192, 168, 1, 2 }));
        });

    test("ip4::address::address(nullptr)", [&]() {
        ip4::address a(nullptr);
        check(a.value() == localhost_address);
        });

    test("ip4::address::address(\"\")", [&]() {
        ip4::address a("");
        check(a.value() == localhost_address);
        });

    test("ip4::address::address(\"192.168.1.2\")", []() {
        ip4::address a("192.168.1.2");
        check(a.value() == htonl(0xc0a80102));
        });

    test("ip4::address::address(<localhost name>)", [&]() {
        ip4::address a(localhost_name);
        check(a.value() == localhost_address);
        });

    test("ip4::address::address(\"192.168.A.2\")", []() {
        check_exception(ip4::address("192.168.A.2"), std::runtime_error);
        });

    test("ip4::address::operator = (value_type)", []() {
        ip4::address a;
        a = htonl(0xc0a80102);
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4::address::bytes_type{192, 168, 1, 2}));
        });

    test("ip4::address::operator = (bytes_type)", []() {
        ip4::address a;
        a = ip4::address::bytes_type{ 192, 168, 1, 2 };
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4::address::bytes_type{ 192, 168, 1, 2 }));
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

    test("ip4::address::operator = (\"192.168.1.2\")", []() {
        ip4::address a;
        a = "192.168.1.2";
        check(a.value() == htonl(0xc0a80102));
        });

    test("ip4::address::operator = (<localhost name>)", [&]() {
        ip4::address a;        
        a = localhost_name;
        check(a.value() == localhost_address);
        });

    test("ip4::address::operator = (\"192.168.A.2\")", []() {
        ip4::address a;
        check_exception(a = "192.168.A.2", std::runtime_error);
        });

    test("ip4::address()::to_string()", []() {
        check(ip4::address().to_string() == "0.0.0.0");
        });

    test("ip4::address(value_type)::to_string()", []() {
        check(ip4::address(htonl(0xc0a80102)).to_string() == "192.168.1.2");
        });

    test("ip4::address(bytes_type)::to_string()", []() {
        check(ip4::address(ip4::address::bytes_type{192, 168, 1, 2}).to_string() == "192.168.1.2");
        });

    test("ip4::address(192, 168, 1, 2)::to_string()", []() {
        check(ip4::address(192, 168, 1, 2).to_string() == "192.168.1.2");
        });

    test("ip4::address(nullptr)::to_string()", [&]() {
        check(ip4::address(nullptr).to_string() == localhost_address_string);
        });

    test("ip4::address(\"\")::to_string()", [&]() {
        check(ip4::address("").to_string() == localhost_address_string);
        });

    test("ip4::address(\"192.168.1.2\")::to_string()", [&]() {
        check(ip4::address("192.168.1.2").to_string() == "192.168.1.2");
        });

    test("ip4::address(<localhost name>)::to_string()", [&]() {
        check(ip4::address(localhost_name).to_string() == localhost_address_string);
        });

    test("ip4::address = value_type; to_string()", []() {
        ip4::address a;
        a = htonl(0xc0a80102);
        check(a.to_string() == "192.168.1.2");
        });

    test("ip4::address = bytes_type; to_string()", []() {
        ip4::address a;
        a = ip4::address::bytes_type{ 192, 168, 1, 2 };
        check(a.to_string() == "192.168.1.2");
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

    test("ip4::address = \"192.168.1.2\"; to_string()", [&]() {
        ip4::address a;
        a = "192.168.1.2";
        check(a.to_string() == "192.168.1.2");
        });

    test("ip4::address = <localhost name>; to_string()", [&]() {
        ip4::address a;
        a = localhost_name;
        check(a.to_string() == localhost_address_string);
        });

};


int main() {
    init();
    test_ip4_address();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
