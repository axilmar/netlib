#include "netlib/platform.hpp"
#include <vector>
#include <iostream>
#include <set>
#include <random>
#include <array>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include "testlib.hpp"
#include "netlib/address_family.hpp"
#include "netlib/socket_type.hpp"
#include "netlib/protocol.hpp"
#include "netlib/internet_address.hpp"
#include "netlib/utility.hpp"
#include "netlib/socket_address.hpp"


using namespace netlib;


using namespace testlib;


class address_family_test {
public:
    address_family_test() {
        test("enum address_family", []() {
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
        test("enum socket_type", []() {
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
        test("enum protocol", []() {
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
        test("class internet_address", []() {
            //constants
            check(internet_address::data_size >= std::max(sizeof(in_addr), sizeof(in6_addr)));
            check(internet_address::ipv4_any.to_string() == "0.0.0.0");
            check(internet_address::ipv4_loopback.to_string() == "127.0.0.1");
            check(internet_address::ipv6_any.to_string() == "::");
            check(internet_address::ipv6_loopback.to_string() == "::1");

            //valid addresses/address families/hostnames
            check(internet_address("192.168.1.2", AF_INET).to_string() == "192.168.1.2");
            check(internet_address("192.168.1.2", AF_INET).address_family() == AF_INET);
            check(internet_address("192.168.1.2", AF_INET).size() == sizeof(in_addr));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C", AF_INET6).to_string() == "fe80:cd00:0:cde:1257:0:211e:729c");
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C", AF_INET6).address_family() == AF_INET6);
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C", AF_INET6).size() == sizeof(in6_addr));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1", AF_INET6).to_string() == "fe80:cd00:0:cde:1257:0:211e:729c%1");
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1", AF_INET6).address_family() == AF_INET6);
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1", AF_INET6).size() == sizeof(in6_addr));
            check(check_host_ip(internet_address(get_host_name().c_str(), AF_INET)));
            check(internet_address(get_host_name().c_str(), AF_INET).address_family() == AF_INET);
            check(internet_address(get_host_name().c_str(), AF_INET).size() == sizeof(in_addr));
            check(check_host_ip(internet_address(get_host_name().c_str(), AF_INET6)));
            check(internet_address(get_host_name().c_str(), AF_INET6).address_family() == AF_INET6);
            check(internet_address(get_host_name().c_str(), AF_INET6).size() == sizeof(in6_addr));

            //null/empty addresses
            check(check_host_ip(internet_address(static_cast<const char*>(nullptr), AF_INET)));
            check(check_host_ip(internet_address("", AF_INET)));
            check(check_host_ip(internet_address(static_cast<const char*>(nullptr), AF_INET6)));
            check(check_host_ip(internet_address("", AF_INET6)));

            //invalid addresses/address families/host names
            check_exception(internet_address("192.I68.1.2", AF_INET), std::invalid_argument);
            check_exception(internet_address("FE80:CD00:0000:0CDE:I257:0000:211E:729C", AF_INET6), std::invalid_argument);
            check_exception(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C$1", AF_INET6), std::invalid_argument);
            check_exception(internet_address("192.168.1.2", 255), std::invalid_argument);
            check_exception(internet_address("foobar", AF_INET), std::invalid_argument);
            check_exception(internet_address("foobar", AF_INET6), std::invalid_argument);

            //autodetect addresses
            check(internet_address("192.168.1.2").to_string() == "192.168.1.2");
            check(internet_address("192.168.1.2").address_family() == AF_INET);
            check(internet_address("192.168.1.2").size() == sizeof(in_addr));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C").to_string() == "fe80:cd00:0:cde:1257:0:211e:729c");
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C").address_family() == AF_INET6);
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C").size() == sizeof(in6_addr));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1").to_string() == "fe80:cd00:0:cde:1257:0:211e:729c%1");
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1").address_family() == AF_INET6);
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1").size() == sizeof(in6_addr));

            //comparison operators
            check(internet_address("192.168.1.2") == internet_address("192.168.1.2"));
            check(internet_address("192.168.1.2") != internet_address("192.168.1.3"));
            check(internet_address("192.168.1.2") < internet_address("192.168.1.3"));
            check(internet_address("192.168.1.3") > internet_address("192.168.1.2"));
            check(internet_address("192.168.1.2") <= internet_address("192.168.1.3"));
            check(internet_address("192.168.1.3") <= internet_address("192.168.1.3"));
            check(internet_address("192.168.1.3") >= internet_address("192.168.1.2"));
            check(internet_address("192.168.1.3") >= internet_address("192.168.1.3"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C") == internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C") != internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C") < internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D") > internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C") <= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D") <= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D") >= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D") >= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1") == internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1") != internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1") < internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%2"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%2") > internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1") <= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D%2"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D%2") <= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D%2"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D%2") >= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C%1"));
            check(internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D%2") >= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D%2"));
            check(!(internet_address("192.168.1.2") == internet_address("192.168.1.3")));
            check(!(internet_address("192.168.1.2") == internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729D")));
            check(!(internet_address("192.168.1.2") != internet_address("192.168.1.2")));
            check(!(internet_address("192.168.1.2") > internet_address("192.168.1.3")));
            check(!(internet_address("192.168.1.2") >= internet_address("192.168.1.3")));
            check(!(internet_address("192.168.1.3") < internet_address("192.168.1.2")));
            check(!(internet_address("192.168.1.3") <= internet_address("192.168.1.2")));
            check_exception(internet_address("192.168.1.2") < internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"), std::invalid_argument);
            check_exception(internet_address("192.168.1.2") <= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"), std::invalid_argument);
            check_exception(internet_address("192.168.1.2") > internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"), std::invalid_argument);
            check_exception(internet_address("192.168.1.2") >= internet_address("FE80:CD00:0000:0CDE:1257:0000:211E:729C"), std::invalid_argument);

            //hostname
            check(internet_address("", AF_INET).host_name() == get_host_name());
            check(internet_address("", AF_INET6).host_name() == get_host_name());
            });
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


class utility_test {
public:
    utility_test() {
        test("function get_host_name", []() { 
            check(netlib::get_host_name() == internet_address("").host_name()); 
            });

        test("function get_addresses", []() { 
            //addresses from hostname
            std::string hostname = get_host_name();
            std::vector<internet_address> addresses1 = get_addresses(hostname.c_str());
            check(std::find(addresses1.begin(), addresses1.end(), internet_address(hostname.c_str(), AF_INET)) != addresses1.end());
            check(std::find(addresses1.begin(), addresses1.end(), internet_address(hostname.c_str(), AF_INET6)) != addresses1.end());

            //addresses from ""
            std::vector<internet_address> addresses2 = get_addresses("");
            check(std::find(addresses2.begin(), addresses2.end(), internet_address("", AF_INET)) != addresses2.end());
            check(std::find(addresses2.begin(), addresses2.end(), internet_address("", AF_INET6)) != addresses2.end());

            //invalid addresses
            check_exception(get_addresses("foobar"), std::invalid_argument);
            check_exception(get_addresses(nullptr), std::invalid_argument);
            });
    }
};


class socket_address_test {
public:
    socket_address_test() {
        test("class socket_address", []() {
            //ipv4
            check(socket_address("192.168.1.1", 10000).address() == "192.168.1.1");
            check(socket_address("192.168.1.1", 10000).address_family() == AF_INET);
            check(socket_address("192.168.1.1", 10000).port() == 10000);
            check(socket_address("192.168.1.1", 10000).size() == sizeof(sockaddr_in));
            check(socket_address("192.168.1.1", 10000).to_string() == "192.168.1.1:10000");

            //ipv6
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000).address() == "fe80:cd00:0:cde:1257:0:211e:729c");
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000).address_family() == AF_INET6);
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000).port() == 10000);
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000).size() == sizeof(sockaddr_in6));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000).to_string() == "fe80:cd00:0:cde:1257:0:211e:729c:10000");
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c%1", 10000).address() == "fe80:cd00:0:cde:1257:0:211e:729c%1");
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c%1", 10000).address_family() == AF_INET6);
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c%1", 10000).port() == 10000);
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c%1", 10000).size() == sizeof(sockaddr_in6));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c%1", 10000).to_string() == "fe80:cd00:0:cde:1257:0:211e:729c%1:10000");
            });
    }
};


int main() {
    init();
    address_family_test();
    socket_type_test();
    protocol_test();
    internet_address_test();
    utility_test();
    socket_address_test();
    cleanup();

    system("pause");
    return static_cast<int>(test_error_count);
}
