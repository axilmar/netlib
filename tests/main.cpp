#include "netlib/platform.hpp"
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
#include "netlib/address_family.hpp"
#include "netlib/socket_type.hpp"
#include "netlib/protocol.hpp"
#include "netlib/internet_address.hpp"
#include "netlib/utility.hpp"
#include "netlib/socket_address.hpp"
#include "netlib/socket.hpp"
#include "netlib/serialization.hpp"
#include "netlib/message.hpp"
#include "netlib/message_io.hpp"


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
            check(socket_address("192.168.1.1", 10000) == socket_address("192.168.1.1", 10000));
            check(!(socket_address("192.168.1.1", 10000) == socket_address("192.168.1.2", 10000)));
            check(socket_address("192.168.1.1", 10000) != socket_address("192.168.1.2", 10000));
            check(!(socket_address("192.168.1.1", 10000) != socket_address("192.168.1.1", 10000)));
            check(socket_address("192.168.1.1", 10000) < socket_address("192.168.1.2", 10000));
            check(!(socket_address("192.168.1.1", 10000) < socket_address("192.168.1.1", 10000)));
            check(socket_address("192.168.1.2", 10000) > socket_address("192.168.1.1", 10000));
            check(!(socket_address("192.168.1.2", 10000) > socket_address("192.168.1.2", 10000)));
            check(socket_address("192.168.1.1", 10000) <= socket_address("192.168.1.2", 10000));
            check(!(socket_address("192.168.1.2", 10000) <= socket_address("192.168.1.1", 10000)));
            check(socket_address("192.168.1.2", 10000) >= socket_address("192.168.1.1", 10000));
            check(!(socket_address("192.168.1.1", 10000) >= socket_address("192.168.1.2", 10000)));
            check(socket_address("192.168.1.1", 10000) <= socket_address("192.168.1.1", 10000));
            check(!(socket_address("192.168.1.2", 10000) <= socket_address("192.168.1.1", 10000)));
            check(socket_address("192.168.1.1", 10000) >= socket_address("192.168.1.1", 10000));
            check(!(socket_address("192.168.1.1", 10000) >= socket_address("192.168.1.2", 10000)));

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
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) == socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) == socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) != socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) != socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) < socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) < socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000) > socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000) > socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) <= socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000) <= socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000) >= socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) >= socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) <= socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000) <= socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000)));
            check(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) >= socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000));
            check(!(socket_address("fe80:cd00:0:cde:1257:0:211e:729c", 10000) >= socket_address("fe80:cd00:0:cde:1257:0:211e:729d", 10000)));
            });
    }
};


class socket_test {
public:
    socket_test() {
        test("class socket", []() {
            invalid_socket_test();
            tcp_socket_test();
            udp_socket_test();
            });
    }

private:
    static void invalid_socket_test() {
        netlib::socket s;
        check(!s);
    }

    static constexpr size_t test_message_count = 10;
    static inline const char message[] = "hello world";

    static void tcp_socket_test() {
        std::thread server_thread([] {
            try {
                netlib::socket s(address_family::ipv4, socket_type::stream);

                s.bind(socket_address({ "", address_family::ipv4 }, 10000));
                s.listen();

                auto [client_socket, client_address] = s.accept();

                for (size_t i = 0; i < test_message_count; ++i) {
                    client_socket.send(message, sizeof(message) - 1);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by server: " << ex.what() << std::endl;
            }
            });

        std::thread client_thread([] {
            try {
                netlib::socket s(address_family::ipv4, socket_type::stream);

                s.connect(socket_address({ "", address_family::ipv4 }, 10000));

                for (size_t i = 0; i < test_message_count; ++i) {
                    char buf[32];

                    const size_t size = s.receive(buf, sizeof(message) - 1);

                    check(size == sizeof(message) - 1);
                    check(strncmp(buf, message, size) == 0);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by client: " << ex.what() << std::endl;
            }
            });

        client_thread.join();
        server_thread.join();
    }

    static void udp_socket_test() {
        socket_address addr({ "", address_family::ipv4 }, 10000);
        netlib::socket s(address_family::ipv4, socket_type::datagram);
        s.bind(addr);

        std::thread server_thread([&] {
            try {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                for (size_t i = 0; i < test_message_count; ++i) {
                    s.send(message, sizeof(message) - 1, addr);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by server: " << ex.what() << std::endl;
            }
            });

        std::thread client_thread([&] {
            try {
                socket_address receive_addr;

                for (size_t i = 0; i < test_message_count; ++i) {
                    char buf[32];

                    const size_t size = s.receive(buf, sizeof(buf), receive_addr);

                    check(size == sizeof(message) - 1);
                    check(strncmp(buf, message, size) == 0);
                    check(receive_addr == addr);
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by client: " << ex.what() << std::endl;
            }
            });

        client_thread.join();
        server_thread.join();
    }
};


class serialization_test {
public:
    serialization_test() {
        test("serialization", []() {
            trivial_serialization_test();
            trivial_array_serialization_test();
            stl_serialization_test();
            });
    }

private:
    struct trivial {
        int x;
        int y;
    };

    struct non_trivial {
        int x;
        int y;

        non_trivial() {}

        non_trivial(int x, int y) : x(x), y(y) {}

        template <class Obj, class F> static void visit(Obj&& obj, F&& func) {
            apply_to_each_value(func, obj.x, obj.y);
        }

        void serialize(std::vector<char>& buffer) const {
            visit(*this, [&](const auto& v) { netlib::serialize(buffer, v); });
        }

        void deserialize(const std::vector<char>& buffer, size_t& pos) {
            visit(*this, [&](auto& v) { netlib::deserialize(buffer, pos, v); });
        }

        bool operator == (const non_trivial& other) const {
            return x == other.x && y == other.y;
        }
    };

    static void trivial_serialization_test() {
        std::vector<char> buffer;

        const int8_t i8 = -1;
        const int16_t i16 = -10;
        const int32_t i32 = -100;
        const int32_t i64 = -1000;
        const uint8_t u8 = 0x01;
        const uint16_t u16 = 0x0201;
        const uint32_t u32 = 0x4030201;
        const uint64_t u64 = 0x0d0c0b0a4030201;
        const float f32 = 3.14f;
        const double f64 = 6.28;
        const bool b = true;
        const trivial t{1, 2};
        const non_trivial nt{3, 4};

        serialize(buffer, i8);
        serialize(buffer, i16);
        serialize(buffer, i32);
        serialize(buffer, i64);
        serialize(buffer, u8);
        serialize(buffer, u16);
        serialize(buffer, u32);
        serialize(buffer, u64);
        serialize(buffer, f32);
        serialize(buffer, f64);
        serialize(buffer, b);
        serialize(buffer, t);
        serialize(buffer, nt);

        check(buffer.size() ==
            sizeof(i8) + sizeof(i16) + sizeof(i32) + sizeof(i64) +
            sizeof(u8) + sizeof(u16) + sizeof(u32) + sizeof(u64) +
            sizeof(f32) + sizeof(f64) +
            1 +
            sizeof(t) +
            sizeof(nt));

        size_t pos = 0;

        int8_t ri8;
        int16_t ri16;
        int32_t ri32;
        int32_t ri64;
        uint8_t ru8;
        uint16_t ru16;
        uint32_t ru32;
        uint64_t ru64;
        float rf32;
        double rf64;
        bool rb;
        trivial rt;
        non_trivial rnt;

        deserialize(buffer, pos, ri8);
        deserialize(buffer, pos, ri16);
        deserialize(buffer, pos, ri32);
        deserialize(buffer, pos, ri64);
        deserialize(buffer, pos, ru8);
        deserialize(buffer, pos, ru16);
        deserialize(buffer, pos, ru32);
        deserialize(buffer, pos, ru64);
        deserialize(buffer, pos, rf32);
        deserialize(buffer, pos, rf64);
        deserialize(buffer, pos, rb);
        deserialize(buffer, pos, rt);
        deserialize(buffer, pos, rnt);

        check(i8 == ri8);
        check(i16 == ri16);
        check(i32 == ri32);
        check(i64 == ri64);
        check(u8 == ru8);
        check(u16 == ru16);
        check(u32 == ru32);
        check(u64 == ru64);
        check(f32 == rf32);
        check(f64 == rf64);
        check(b == rb);
        check(t.x == rt.x && t.y == rt.y);
        check(nt == rnt);
    }

    static void trivial_array_serialization_test() {
        std::vector<char> buffer;

        const int8_t i8[] = { -1, -2 };
        const int16_t i16[] = { -10, -20 };
        const int32_t i32[] = { -100, -200 };
        const int32_t i64[] = { -1000, -2000 };
        const uint8_t u8[] = { 0x01, 0x02 };
        const uint16_t u16[] = { 0x0201, 0x0102 };
        const uint32_t u32[] = { 0x4030201, 0x01020304 };
        const uint64_t u64[] = { 0x0d0c0b0a8040201, 0x010204080a0b0c0d };
        const float f32[] = { 3.14f, 6.28f };
        const double f64[] = { 6.28, 12.56 };
        const bool b[] = { true, false };
        const trivial t[] = { { 1, 2 }, {3, 4} };
        const non_trivial nt[] = { {5, 6}, {7, 8} };

        serialize(buffer, i8, size_t(2));
        serialize(buffer, i16, size_t(2));
        serialize(buffer, i32, size_t(2));
        serialize(buffer, i64, size_t(2));
        serialize(buffer, u8, size_t(2));
        serialize(buffer, u16, size_t(2));
        serialize(buffer, u32, size_t(2));
        serialize(buffer, u64, size_t(2));
        serialize(buffer, f32, size_t(2));
        serialize(buffer, f64, size_t(2));
        serialize(buffer, b, size_t(2));
        serialize(buffer, t, size_t(2));
        serialize(buffer, nt, size_t(2));

        check(buffer.size() == 
            sizeof(i8) + sizeof(i16) + sizeof(i32) + sizeof(i64) + 
            sizeof(u8) + sizeof(u16) + sizeof(u32) + sizeof(u64) + 
            sizeof(f32) + sizeof(f64) + 
            (((sizeof(b) / sizeof(bool)) + CHAR_BIT - 1) / CHAR_BIT) + 
            sizeof(t) +
            sizeof(nt));

        size_t pos = 0;

        int8_t ri8[2];
        int16_t ri16[2];
        int32_t ri32[2];
        int32_t ri64[2];
        uint8_t ru8[2];
        uint16_t ru16[2];
        uint32_t ru32[2];
        uint64_t ru64[2];
        float rf32[2];
        double rf64[2];
        bool rb[2];
        trivial rt[2];

        deserialize(buffer, pos, ri8, size_t(2));
        deserialize(buffer, pos, ri16, size_t(2));
        deserialize(buffer, pos, ri32, size_t(2));
        deserialize(buffer, pos, ri64, size_t(2));
        deserialize(buffer, pos, ru8, size_t(2));
        deserialize(buffer, pos, ru16, size_t(2));
        deserialize(buffer, pos, ru32, size_t(2));
        deserialize(buffer, pos, ru64, size_t(2));
        deserialize(buffer, pos, rf32, size_t(2));
        deserialize(buffer, pos, rf64, size_t(2));
        deserialize(buffer, pos, rb, size_t(2));
        deserialize(buffer, pos, rt, size_t(2));

        check(memcmp(i8, ri8, sizeof(i8)) == 0);
        check(memcmp(i16, ri16, sizeof(i16)) == 0);
        check(memcmp(i32, ri32, sizeof(i32)) == 0);
        check(memcmp(i64, ri64, sizeof(i64)) == 0);
        check(memcmp(u8, ru8, sizeof(u8)) == 0);
        check(memcmp(u16, ru16, sizeof(u16)) == 0);
        check(memcmp(u32, ru32, sizeof(u32)) == 0);
        check(memcmp(u64, ru64, sizeof(u64)) == 0);
        check(memcmp(f32, rf32, sizeof(f32)) == 0);
        check(memcmp(f64, rf64, sizeof(f64)) == 0);
        check(memcmp(b, rb, sizeof(b)) == 0);
        check(memcmp(t, rt, sizeof(t)) == 0);
    }

    static void stl_serialization_test() {
        std::vector<char> buffer;

        const std::pair<int, double> p1{ 1, 3.14 };
        const std::tuple<int, double, char> t1{ 1, 3.14, 'c' };
        const std::optional<int> opt1{ 1 };
        const std::optional<int> opt2;
        const std::variant<int, double, char> v1 = 3.14;
        const std::array<int, 3> arr1{1, 2, 3};
        const std::string str1{"hello world!!!"};
        const std::vector<int> vec1{4, 5, 6};
        const std::deque<int> dq1{ 7, 8, 9 };
        const std::forward_list<int> fl1{ 10, 11, 12 };
        const std::list<int> list1{ 13, 14, 15 };
        const std::set<int> set1{ 16, 17, 18 };
        const std::multiset<int> set2{ 19, 20, 21 };
        const std::map<int, int> map1{ std::pair(0, 0), std::pair(1, 1) };
        const std::multimap<int, int> map2{ std::pair(2, 2), std::pair(3, 3) };
        const std::unordered_set<int> uset1{ 16, 17, 18 };
        const std::unordered_multiset<int> uset2{ 19, 20, 21 };
        const std::unordered_map<int, int> umap1{ std::pair(0, 0), std::pair(1, 1) };
        const std::unordered_multimap<int, int> umap2{ std::pair(2, 2), std::pair(3, 3) };

        serialize(buffer, p1);
        serialize(buffer, t1);
        serialize(buffer, opt1, opt2);
        serialize(buffer, v1);
        serialize(buffer, arr1);
        serialize(buffer, str1);
        serialize(buffer, vec1);
        serialize(buffer, dq1);
        serialize(buffer, fl1);
        serialize(buffer, list1);
        serialize(buffer, set1);
        serialize(buffer, set2);
        serialize(buffer, map1);
        serialize(buffer, map2);
        serialize(buffer, uset1);
        serialize(buffer, uset2);
        serialize(buffer, umap1);
        serialize(buffer, umap2);

        size_t pos = 0;

        std::pair<int, double> rp1;
        std::tuple<int, double, char> rt1;
        std::optional<int> ropt1;
        std::optional<int> ropt2;
        std::variant<int, double, char> rv1;
        std::array<int, 3> rarr1;
        std::string rstr1;
        std::vector<int> rvec1;
        std::deque<int> rdq1;
        std::forward_list<int> rfl1;
        std::list<int> rlist1;
        std::set<int> rset1;
        std::multiset<int> rset2;
        std::map<int, int> rmap1;
        std::multimap<int, int> rmap2;
        std::unordered_set<int> ruset1;
        std::unordered_multiset<int> ruset2;
        std::unordered_map<int, int> rumap1;
        std::unordered_multimap<int, int> rumap2;

        deserialize(buffer, pos, rp1);
        deserialize(buffer, pos, rt1);
        deserialize(buffer, pos, ropt1, ropt2);
        deserialize(buffer, pos, rv1);
        deserialize(buffer, pos, rarr1);
        deserialize(buffer, pos, rstr1);
        deserialize(buffer, pos, rvec1);
        deserialize(buffer, pos, rdq1);
        deserialize(buffer, pos, rfl1);
        deserialize(buffer, pos, rlist1);
        deserialize(buffer, pos, rset1);
        deserialize(buffer, pos, rset2);
        deserialize(buffer, pos, rmap1);
        deserialize(buffer, pos, rmap2);
        deserialize(buffer, pos, ruset1);
        deserialize(buffer, pos, ruset2);
        deserialize(buffer, pos, rumap1);
        deserialize(buffer, pos, rumap2);

        check(p1 == rp1);
        check(t1 == rt1);
        check(opt1 == ropt1 && opt2 == ropt2);
        check(v1 == rv1);
        check(arr1 == rarr1);
        check(str1 == rstr1);
        check(vec1 == rvec1);
        check(dq1 == rdq1);
        check(fl1 == rfl1);
        check(list1 == rlist1);
        check(set1 == rset1);
        check(set2 == rset2);
        check(map1 == rmap1);
        check(map2 == rmap2);
        check(uset1 == ruset1);
        check(uset2 == ruset2);
        check(umap1 == rumap1);
        check(umap2 == rumap2);
    }
};


class message_test {
public:
    message_test() {
        test("message I/O", []() {
            tcp_test();
            udp_test();
            });
    }

private:
    using test_message = message<size_t, std::string, double>;

    static constexpr size_t test_message_count = 10;

    static void tcp_test() {
        std::thread server_thread([] {
            try {
                netlib::socket s(address_family::ipv4, socket_type::stream);

                s.bind(socket_address({ "", address_family::ipv4 }, 10000));
                s.listen();

                auto [client_socket, client_address] = s.accept();

                for (size_t i = 0; i < test_message_count; ++i) {
                    tcp_send_message(client_socket, test_message(i, "hello world!!!", 3.14));
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by server: " << ex.what() << std::endl;
            }
            });

        std::thread client_thread([] {
            try {
                netlib::socket s(address_family::ipv4, socket_type::stream);

                s.connect(socket_address({ "", address_family::ipv4 }, 10000));

                for (size_t i = 0; i < test_message_count; ++i) {
                    message_ptr msg = tcp_receive_message(s);
                    if (msg->message_id() == test_message::id) {
                        test_message& tm = dynamic_cast<test_message&>(*msg);
                        check(std::get<0>(tm) == i);
                        check(std::get<1>(tm) == "hello world!!!");
                        check(std::get<2>(tm) == 3.14);
                    }
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by client: " << ex.what() << std::endl;
            }
            });

        client_thread.join();
        server_thread.join();
    }

    static void udp_test() {
        socket_address addr({ "", address_family::ipv4 }, 10000);
        netlib::socket s(address_family::ipv4, socket_type::datagram);
        s.bind(addr);

        std::thread server_thread([&] {
            try {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                for (size_t i = 0; i < test_message_count; ++i) {
                    udp_send_message(s, addr, test_message(i, "hello world!!!", 3.14));
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by server: " << ex.what() << std::endl;
            }
            });

        std::thread client_thread([&] {
            try {
                socket_address receive_addr;

                for (size_t i = 0; i < test_message_count; ++i) {
                    message_ptr msg = udp_receive_message(s, receive_addr);
                    if (msg->message_id() == test_message::id) {
                        test_message& tm = dynamic_cast<test_message&>(*msg);
                        check(std::get<0>(tm) == i);
                        check(std::get<1>(tm) == "hello world!!!");
                        check(std::get<2>(tm) == 3.14);
                    }
                }
            }
            catch (const std::exception& ex) {
                std::cout << "Exception thrown by client: " << ex.what() << std::endl;
            }
            });

        client_thread.join();
        server_thread.join();
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
    socket_test();
    serialization_test();
    message_test();
    cleanup();

    system("pause");
    return static_cast<int>(test_error_count);
}
