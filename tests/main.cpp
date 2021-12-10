#include "../src/netlib/platform.hpp"
#include <unordered_set>
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
    std::string localhost_ip4_string;
    std::string localhost_ip6_string;
    getaddrinfo(localhost_name, nullptr, nullptr, &ai);
    for (addrinfo* tai = ai; tai; tai = tai->ai_next) {
        if (tai->ai_family == AF_INET) {
            if (!ip4_found) {
                ip4_found = true;
                localhost_ip4_bytes = reinterpret_cast<const std::array<char, 4>&>(reinterpret_cast<const sockaddr_in*>(tai->ai_addr)->sin_addr);
                localhost_ip4_value = reinterpret_cast<const uint32_t&>(reinterpret_cast<const sockaddr_in*>(tai->ai_addr)->sin_addr);
                char buffer[256];
                inet_ntop(AF_INET, &reinterpret_cast<const sockaddr_in*>(tai->ai_addr)->sin_addr, buffer, sizeof(buffer));
                localhost_ip4_string = buffer;
            }
        }
        else if (tai->ai_family == AF_INET6) {
            if (!ip6_found) {
                ip6_found = true;
                localhost_ip6_bytes = reinterpret_cast<const std::array<char, 16>&>(reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_addr);
                localhost_ip6_words = reinterpret_cast<const std::array<uint16_t, 8>&>(reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_addr);
                localhost_zone_index = reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_scope_id;
                char buffer[256];
                inet_ntop(AF_INET6, &reinterpret_cast<const sockaddr_in6*>(tai->ai_addr)->sin6_addr, buffer, sizeof(buffer));
                localhost_ip6_string = std::string(buffer) + ((localhost_zone_index == 0) ? "" : '%' + std::to_string(localhost_zone_index));
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

    test("ip_address::to_string())", [&]() {
        check(ip_address(nullptr, ip_address::ip4).to_string() == localhost_ip4_string);
        check(ip_address(nullptr, ip_address::ip6).to_string() == localhost_ip6_string);
        });

    test("ip_address::compare())", [&]() {
        {
            const std::string ip4_less = "10.11.12.12";
            const std::string ip4_greater = "10.11.12.14";
            check(ip_address(ip4_string) == ip_address(ip4_string));
            check(ip_address(ip4_string) != ip_address(ip4_less));
            check(ip_address(ip4_less) < ip_address(ip4_string));
            check(ip_address(ip4_greater) > ip_address(ip4_string));
            check(ip_address(ip4_less) <= ip_address(ip4_string));
            check(ip_address(ip4_string) <= ip_address(ip4_string));
            check(ip_address(ip4_greater) >= ip_address(ip4_string));
            check(ip_address(ip4_string) >= ip_address(ip4_string));
        }
        {
            const std::string ip6_less = "0001:0203:0405:0607:0809:0a0b:0c0d:0e0e";
            const std::string ip6_greater = "0001:0203:0405:0607:0809:0a0b:0c0d:0f0f";
            check(ip_address(ip6_string) == ip_address(ip6_string));
            check(ip_address(ip6_string) != ip_address(ip6_less));
            check(ip_address(ip6_less) < ip_address(ip6_string));
            check(ip_address(ip6_greater) > ip_address(ip6_string));
            check(ip_address(ip6_less) <= ip_address(ip6_string));
            check(ip_address(ip6_string) <= ip_address(ip6_string));
            check(ip_address(ip6_greater) >= ip_address(ip6_string));
            check(ip_address(ip6_string) >= ip_address(ip6_string));
        }
        {
            const std::string ip6_less = "0001:0203:0405:0607:0809:0a0b:0c0d:0e0f%0";
            const std::string ip6_greater = "0001:0203:0405:0607:0809:0a0b:0c0d:0e0f%2";
            check(ip_address(ip6_string_1) == ip_address(ip6_string_1));
            check(ip_address(ip6_string_1) != ip_address(ip6_less));
            check(ip_address(ip6_less) < ip_address(ip6_string_1));
            check(ip_address(ip6_greater) > ip_address(ip6_string_1));
            check(ip_address(ip6_less) <= ip_address(ip6_string_1));
            check(ip_address(ip6_string_1) <= ip_address(ip6_string_1));
            check(ip_address(ip6_greater) >= ip_address(ip6_string_1));
            check(ip_address(ip6_string_1) >= ip_address(ip6_string_1));
        }
        });

    test("ip_address::hash())", [&]() {
        std::unordered_set<ip_address> s;
        s.insert(ip_address(ip4_string));
        s.insert(ip_address(ip6_string));
        check(s.size() == 2);
        check(s.find(ip_address(ip4_string)) != s.end());
        check(s.find(ip_address(ip6_string)) != s.end());
        check(s.find(ip_address(ip4_string)) != s.find(ip_address(ip6_string)));
        });
}


int main() {
    init();
    test_ip_address();
    cleanup();
    system("pause");
    return static_cast<int>(test_error_count);
}
