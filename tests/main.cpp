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

    ip4_address::value_type localhost_address;
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

    test("ip4_address::ip4_address()", []() {
        ip4_address a;
        check(a.value() == 0);
        check(a.bytes() == ip4_address::bytes_type{});
        });

    test("ip4_address::ip4_address(value_type)", []() {
        ip4_address a(htonl(0xc0a80102));
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4_address::bytes_type{192, 168, 1, 2}));
        });

    test("ip4_address::ip4_address(bytes_type)", []() {
        ip4_address a(ip4_address::bytes_type{ 192, 168, 1, 2 });
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4_address::bytes_type{ 192, 168, 1, 2 }));
        });

    test("ip4_address::ip4_address(192, 168, 1, 2)", []() {
        ip4_address a(192, 168, 1, 2);
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4_address::bytes_type{ 192, 168, 1, 2 }));
        });

    test("ip4_address::ip4_address(nullptr)", [&]() {
        ip4_address a(nullptr);
        check(a.value() == localhost_address);
        });

    test("ip4_address::ip4_address(\"\")", [&]() {
        ip4_address a("");
        check(a.value() == localhost_address);
        });

    test("ip4_address::ip4_address(\"192.168.1.2\")", []() {
        ip4_address a("192.168.1.2");
        check(a.value() == htonl(0xc0a80102));
        });

    test("ip4_address::ip4_address(<localhost name>)", [&]() {
        ip4_address a(localhost_name);
        check(a.value() == localhost_address);
        });

    test("ip4_address::ip4_address(\"192.168.A.2\")", []() {
        check_exception(ip4_address("192.168.A.2"), std::invalid_argument);
        });

    test("ip4_address::operator = (value_type)", []() {
        ip4_address a;
        a = htonl(0xc0a80102);
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4_address::bytes_type{192, 168, 1, 2}));
        });

    test("ip4_address::operator = (bytes_type)", []() {
        ip4_address a;
        a = ip4_address::bytes_type{ 192, 168, 1, 2 };
        check(a.value() == htonl(0xc0a80102));
        check((a.bytes() == ip4_address::bytes_type{ 192, 168, 1, 2 }));
        });

    test("ip4_address::operator = (nullptr)", [&]() {
        ip4_address a;        
        a = nullptr;
        check(a.value() == localhost_address);
        });

    test("ip4_address::operator = (\"\")", [&]() {
        ip4_address a;
        a = "";
        check(a.value() == localhost_address);
        });

    test("ip4_address::operator = (\"192.168.1.2\")", []() {
        ip4_address a;
        a = "192.168.1.2";
        check(a.value() == htonl(0xc0a80102));
        });

    test("ip4_address::operator = (<localhost name>)", [&]() {
        ip4_address a;        
        a = localhost_name;
        check(a.value() == localhost_address);
        });

    test("ip4_address::operator = (\"192.168.A.2\")", []() {
        ip4_address a;
        check_exception(a = "192.168.A.2", std::invalid_argument);
        });

    test("ip4_address()::to_string()", []() {
        check(ip4_address().to_string() == "0.0.0.0");
        });

    test("ip4_address(value_type)::to_string()", []() {
        check(ip4_address(htonl(0xc0a80102)).to_string() == "192.168.1.2");
        });

    test("ip4_address(bytes_type)::to_string()", []() {
        check(ip4_address(ip4_address::bytes_type{192, 168, 1, 2}).to_string() == "192.168.1.2");
        });

    test("ip4_address(192, 168, 1, 2)::to_string()", []() {
        check(ip4_address(192, 168, 1, 2).to_string() == "192.168.1.2");
        });

    test("ip4_address(nullptr)::to_string()", [&]() {
        check(ip4_address(nullptr).to_string() == localhost_address_string);
        });

    test("ip4_address(\"\")::to_string()", [&]() {
        check(ip4_address("").to_string() == localhost_address_string);
        });

    test("ip4_address(\"192.168.1.2\")::to_string()", [&]() {
        check(ip4_address("192.168.1.2").to_string() == "192.168.1.2");
        });

    test("ip4_address(<localhost name>)::to_string()", [&]() {
        check(ip4_address(localhost_name).to_string() == localhost_address_string);
        });

    test("ip4_address = value_type; to_string()", []() {
        ip4_address a;
        a = htonl(0xc0a80102);
        check(a.to_string() == "192.168.1.2");
        });

    test("ip4_address = bytes_type; to_string()", []() {
        ip4_address a;
        a = ip4_address::bytes_type{ 192, 168, 1, 2 };
        check(a.to_string() == "192.168.1.2");
        });

    test("ip4_address = nullptr; to_string()", [&]() {
        ip4_address a;
        a = nullptr;
        check(a.to_string() == localhost_address_string);
        });

    test("ip4_address = \"\"; to_string()", [&]() {
        ip4_address a;
        a = "";
        check(a.to_string() == localhost_address_string);
        });

    test("ip4_address = \"192.168.1.2\"; to_string()", [&]() {
        ip4_address a;
        a = "192.168.1.2";
        check(a.to_string() == "192.168.1.2");
        });

    test("ip4_address = <localhost name>; to_string()", [&]() {
        ip4_address a;
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
