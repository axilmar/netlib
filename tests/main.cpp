#include "../src/netlib/platform.hpp"
#include "testlib.hpp"
#include "netlib/ip_address.hpp"


using namespace testlib;
using namespace netlib;


static void test_ip_address() {
    const uint32_t ip4_value = 0x0d0c0b0a;
    const std::string ip4_string = "10.11.12.13";
    const std::array<char, 4> ip4_bytes{0x0a, 0x0b, 0x0c, 0x0d};
    const std::array<char, 16> ip6_bytes{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    const std::array<uint16_t, 8> ip6_words{ 0x0100, 0x0302, 0x0504, 0x0706, 0x0908, 0x0b0a, 0x0d0c, 0x0f0e };
    const std::string ip6_string("0001:0203:0405:0607:0809:0a0b:0c0d:0e0f");
    const std::string ip6_string_1("0001:0203:0405:0607:0809:0a0b:0c0d:0e0f%1");

    std::array<char, 4> localhost_ip4_bytes;
    uint32_t localhost_ip4_value;
    std::array<char, 16> localhost_ip6_bytes;
    std::array<uint16_t, 8> localhost_ip6_words;
    uint32_t localhost_zone_index;
    char localhost_name[257];
    localhost_name[256] = '\0';
    gethostname(localhost_name, sizeof(localhost_name) - 1);
    addrinfo* ai;
    bool ip4_found = false, ip6_found = false;
    getaddrinfo(localhost_name, nullptr, nullptr, &ai);
    for (addrinfo* tai = ai; tai; tai = tai->ai_next) {
        if (tai->ai_family == AF_INET) {
            if (!ip4_found) {
                ip4_found = true;
                localhost_ip4_bytes = reinterpret_cast<const std::array<char, 4>&>(reinterpret_cast<const sockaddr_in*>(tai->ai_addr)->sin_addr);
                localhost_ip4_value = reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(tai->ai_addr)->sin_addr);
            }
        }
        else if (tai->ai_family == AF_INET6) {
            if (!ip6_found) {
                ip6_found = true;
                localhost_ip6_bytes = reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_addr);
                localhost_ip6_words = reinterpret_cast<const std::array<uint16_t, 8>&>(reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_addr);
                localhost_zone_index = reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_scope_id;
            }
        }
        if (ip4_found && ip6_found) {
            break;
        }
    }
    freeaddrinfo(ai);

    test("ip_address::ip_address()", [&]() {
        ip_address a;
        std::array<char, 4> b{};
        check(a.type() == ip_address::ip4);
        check(a.ip4_bytes() == b);
        });

    test("ip_address::ip_address(ip4 value)", [&]() {
        ip_address a(ip4_value);
        check(a.type() == ip_address::ip4);
        check(a.ip4_value() == ip4_value);
        });

    test("ip_address::ip_address(ip4 bytes)", [&]() {
        ip_address a(ip4_bytes);
        check(a.type() == ip_address::ip4);
        check(a.ip4_bytes() == ip4_bytes);
        });

    test("ip_address::ip_address(ip6 bytes, 1)", [&]() {
        ip_address a(ip6_bytes, 1);
        check(a.type() == ip_address::ip6);
        check(a.ip6_bytes() == ip6_bytes);
        check(a.zone_index() == 1);
        });

    test("ip_address::ip_address(ip6 words, 1)", [&]() {
        ip_address a(ip6_words, 1);
        check(a.type() == ip_address::ip6);
        check(a.ip6_words() == ip6_words);
        check(a.zone_index() == 1);
        });

    test("ip_address::ip_address(nullptr, ip4)", [&]() {
        ip_address a(nullptr, ip_address::ip4);
        check(a.type() == ip_address::ip4);
        check(a.ip4_bytes() == localhost_ip4_bytes);
        check(a.ip4_value() == localhost_ip4_value);
        });

    test("ip_address::ip_address("", ip4)", [&]() {
        ip_address a("", ip_address::ip4);
        check(a.type() == ip_address::ip4);
        check(a.ip4_bytes() == localhost_ip4_bytes);
        check(a.ip4_value() == localhost_ip4_value);
        });

    test("ip_address::ip_address(ip4 string, ip4)", [&]() {
        ip_address a(ip4_string, ip_address::ip4);
        check(a.type() == ip_address::ip4);
        check(a.ip4_bytes() == ip4_bytes);
        check(a.ip4_value() == ip4_value);
        });

    test("ip_address::ip_address(localhost name, ip4)", [&]() {
        ip_address a(localhost_name, ip_address::ip4);
        check(a.type() == ip_address::ip4);
        check(a.ip4_bytes() == localhost_ip4_bytes);
        check(a.ip4_value() == localhost_ip4_value);
        });

    test("ip_address::ip_address(nullptr, ip6)", [&]() {
        ip_address a(nullptr, ip_address::ip6);
        check(a.type() == ip_address::ip6);
        check(a.ip6_bytes() == localhost_ip6_bytes);
        check(a.ip6_words() == localhost_ip6_words);
        });

    test("ip_address::ip_address("", ip6)", [&]() {
        ip_address a("", ip_address::ip6);
        check(a.type() == ip_address::ip6);
        check(a.ip6_bytes() == localhost_ip6_bytes);
        check(a.ip6_words() == localhost_ip6_words);
        });

    test("ip_address::ip_address(ip6 string, ip6)", [&]() {
        ip_address a(ip6_string, ip_address::ip6);
        check(a.type() == ip_address::ip6);
        check(a.ip6_bytes() == ip6_bytes);
        check(a.ip6_words() == ip6_words);
        });

    test("ip_address::ip_address(ip6 string %1, ip6)", [&]() {
        ip_address a(ip6_string_1, ip_address::ip6);
        check(a.type() == ip_address::ip6);
        check(a.ip6_bytes() == ip6_bytes);
        check(a.ip6_words() == ip6_words);
        check(a.zone_index() == 1);
        });

    test("ip_address::ip_address(localhost name, ip6)", [&]() {
        ip_address a(localhost_name, ip_address::ip6);
        check(a.type() == ip_address::ip6);
        check(a.ip6_bytes() == localhost_ip6_bytes);
        check(a.ip6_words() == localhost_ip6_words);
        check(a.zone_index() == localhost_zone_index);
        });

}


int main() {
    init();
    test_ip_address();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
