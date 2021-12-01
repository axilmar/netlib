#include "netlib/platform.hpp"
#include <stdexcept>
#include <algorithm>
#include <string_view>
#include "netlib/internet_address.hpp"


namespace netlib {


    static_assert(internet_address::data_size >= std::max(sizeof(in_addr), sizeof(in6_addr)));


    //constant addresses
    static const auto inaddr_any = ntohl(INADDR_ANY);
    static const auto inaddr_loopback = ntohl(INADDR_LOOPBACK);
    const internet_address internet_address::ipv4_any(&inaddr_any, AF_INET);
    const internet_address internet_address::ipv4_loopback(&inaddr_loopback, AF_INET);
    const internet_address internet_address::ipv6_any(&in6addr_any, AF_INET6);
    const internet_address internet_address::ipv6_loopback(&in6addr_loopback, AF_INET6);


    //get address from hostname
    static int get_address_from_hostname(const char* address, int af, void* addr, uint32_t& ipv6_scope_id) {

        //get address info
        addrinfo* ai;
        int error = getaddrinfo(address, nullptr, nullptr, &ai);

        //throw for error
        if (error) {
            throw std::invalid_argument(get_last_error_message(error));
        }

        addrinfo* start_ai = ai;

        //find address to return
        for (; ai; ai = ai->ai_next) {

            //get ipv4 address
            if (ai->ai_family == AF_INET && (af == 0 || af == AF_INET)) {
                memcpy(addr, &reinterpret_cast<const SOCKADDR_IN*>(ai->ai_addr)->sin_addr, sizeof(in_addr));
                freeaddrinfo(start_ai);
                return AF_INET;
            }

            //get ipv6 address
            if (ai->ai_family == AF_INET6 && (af == 0 || af == AF_INET6)) {
                memcpy(addr, &reinterpret_cast<const SOCKADDR_IN6*>(ai->ai_addr)->sin6_addr, sizeof(in6_addr));
                ipv6_scope_id = reinterpret_cast<const SOCKADDR_IN6*>(ai->ai_addr)->sin6_scope_id;
                freeaddrinfo(start_ai);
                return AF_INET6;
            }
        }

        freeaddrinfo(start_ai);

        //invalid address
        throw std::invalid_argument(std::string("Upsupported internet address: ") + address);
    }


    //parses scope id
    static uint32_t parse_ipv6_scope_id(const char* address) {
        //locate '%'
        const char* percent_pos = strchr(address, '%');

        //if not found, then scope id is 0
        if (!percent_pos) {
            return 0;
        }

        //parse number
        uint32_t result;
        try {
            if constexpr (sizeof(int) == sizeof(uint32_t)) {
                result = std::stoi(percent_pos + 1);
            }
            else if constexpr (sizeof(long) == sizeof(uint32_t)) {
                result = std::stol(percent_pos + 1);
            }
            else {
                static_assert("Neither int nor long is 32-bits.");
            }
        }

        //failed to parse integer; get scope id from name
        catch (const std::invalid_argument&) {
            result = if_nametoindex(percent_pos + 1);
            if (!result) {
                throw std::runtime_error("Could not convert interface name to scope id.");
            }
        }

        return result;
    }


    //parse ipv6 address
    static bool parse_ipv6_address(const char* address, void* data, uint32_t& scope_id) {
        //find percent
        const char* percent_pos = strchr(address, '%');

        //no percent pos, so use inet_pton
        if (!percent_pos) {
            return inet_pton(AF_INET6, address, data) == 1;
        }

        //get raw ipv6 address
        std::string raw_address(address, percent_pos);

        //parse raw address
        if (inet_pton(AF_INET6, raw_address.c_str(), data) != 1) {
            return false;
        }

        //success; parse scope id
        scope_id = parse_ipv6_scope_id(percent_pos);

        return true;
    }


    //get zone name from id
    static std::string get_ipv6_zone_name(uint32_t scope_id) {
        return '%' + std::to_string(scope_id);
    }


    //constructor
    internet_address::internet_address(const char* address, int af) 
        : m_data{}
        , m_ipv6_scope_id{}
    {
        //if address is given
        if (address && strlen(address) > 0) {

            //try to autodetect address family, if not given
            if (af == 0) {
                if (inet_pton(AF_INET, address, m_data) == 1) {
                    m_address_family = AF_INET;
                    return;
                }

                if (parse_ipv6_address(address, m_data, m_ipv6_scope_id)) {
                    m_address_family = AF_INET6;
                    return;
                }
            }

            //get address from hostname
            m_address_family = get_address_from_hostname(address, af, m_data, m_ipv6_scope_id);
            return;
        }

        //the address is not given, get hostname and get its address

        //buffer for hostname; 
        //in case of truncation, the returned string is not-null terminated,
        //so provide one extra slot for null-termination.
        char buf[257];
        buf[256] = '\0';

        //get the host name
        const int result = gethostname(buf, sizeof(buf) - 1);
        if (result == -1) {
            throw std::runtime_error(get_last_error_message());
        }

        //get address from this host
        m_address_family = get_address_from_hostname(buf, af, m_data, m_ipv6_scope_id);
    }


    //internal constructor
    internet_address::internet_address(const void* data, int af, uint32_t ipv6_scope_id)
        : m_address_family(af)
        , m_ipv6_scope_id(ipv6_scope_id)
    {
        if (af == AF_INET) {
            memcpy(m_data, data, sizeof(in_addr));
        }

        memcpy(m_data, data, data_size);
    }


    //Returns the size of the data.
    size_t internet_address::size() const {
        switch (m_address_family) {
        case AF_INET:
            return sizeof(in_addr);

        case AF_INET6:
            return sizeof(in6_addr);
        }

        throw std::runtime_error("Unsupported address family");
    }


    //get scope id
    uint32_t internet_address::get_scope_id() const {
        if (m_address_family == AF_INET6) {
            return m_ipv6_scope_id;
        }
        throw std::runtime_error("Current address family does not support scope id.");
    }


    //converts the internet address to a string.
    std::string internet_address::to_string() const {
        char buf[256];

        const char* str = inet_ntop(m_address_family, m_data, buf, sizeof(buf));

        if (str) {
            switch (m_address_family) {
            case AF_INET6:
                if (m_ipv6_scope_id) {
                    return str + get_ipv6_zone_name(m_ipv6_scope_id);
                }
                return str;

            default:
                return str;
            }
        }

        throw std::runtime_error(get_last_error_message());
    }


    //Checks if this address matches the given address.
    bool internet_address::operator == (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            return false;
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) == 0;
        }

        return memcmp(m_data, other.m_data, data_size) == 0 && m_ipv6_scope_id == other.m_ipv6_scope_id;
    }


    //Checks if this address matches the given address.
    bool internet_address::operator != (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            return true;
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) != 0;
        }

        return memcmp(m_data, other.m_data, data_size) != 0 || m_ipv6_scope_id != other.m_ipv6_scope_id;
    }


    //Checks if this address comes before the given address.
    bool internet_address::operator < (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) < 0;
        }

        const int cmp = memcmp(m_data, other.m_data, data_size);

        if (cmp < 0) {
            return true;
        }

        if (cmp > 0) {
            return false;
        }

        return m_ipv6_scope_id < other.m_ipv6_scope_id;
    }


    //Checks if this address comes after the given address.
    bool internet_address::operator > (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) > 0;
        }

        const int cmp = memcmp(m_data, other.m_data, data_size);

        if (cmp > 0) {
            return true;
        }

        if (cmp < 0) {
            return false;
        }

        return m_ipv6_scope_id > other.m_ipv6_scope_id;
    }


    //Checks if this address comes before the given address or they are equal.
    bool internet_address::operator <= (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) <= 0;
        }

        const int cmp = memcmp(m_data, other.m_data, data_size);

        if (cmp < 0) {
            return true;
        }

        if (cmp > 0) {
            return false;
        }

        return m_ipv6_scope_id <= other.m_ipv6_scope_id;
    }


    //Checks if this address comes after the given address or they are equal.
    bool internet_address::operator >= (const internet_address& other) const {
        if (other.m_address_family != m_address_family) {
            throw std::invalid_argument("Address families do not match.");
        }

        if (m_address_family == AF_INET) {
            return memcmp(m_data, other.m_data, sizeof(in_addr)) >= 0;
        }

        const int cmp = memcmp(m_data, other.m_data, data_size);

        if (cmp > 0) {
            return true;
        }

        if (cmp < 0) {
            return false;
        }

        return m_ipv6_scope_id >= other.m_ipv6_scope_id;
    }


    //get hash value
    size_t internet_address::hash() const noexcept {
        return std::hash<std::string_view>()(std::string_view(m_data, m_address_family == AF_INET ? sizeof(in_addr) : data_size));
    }


    //Checks if the internet address represents a valid node in the network.
    std::string internet_address::host_name() const {
        union {
            sockaddr_storage sa_storage{};
            sockaddr sa;
            sockaddr_in sa4;
            sockaddr_in6 sa6;
        };
        socklen_t sa_size;

        //prepare the socket address
        switch (m_address_family) {
        case AF_INET:
            memcpy(&sa4.sin_addr, m_data, sizeof(in_addr));
            sa4.sin_family = AF_INET;
            sa_size = sizeof(sa4);
            break;

        case AF_INET6:
            memcpy(&sa6.sin6_addr, m_data, sizeof(in6_addr));
            sa6.sin6_family = AF_INET6;
            sa6.sin6_scope_id = m_ipv6_scope_id;
            sa_size = sizeof(sa6);
            break;

        default:
            throw std::runtime_error("Unsupported address family");
        }

        //get the name
        char name[1024];
        int error = getnameinfo(&sa, sa_size, name, sizeof(name), nullptr, 0, 0);

        //no error
        if (!error) {
            return name;
        }

        //the name could not be resolved
        if (error == EAI_NONAME) {
            return std::string();
        }

        //get error string
        std::string error_msg;
        #ifdef _WIN32
        error_msg = get_last_error_message();
        #endif

        #ifdef linux
        switch (error) {
            case EAI_AGAIN:
                error_msg = "The name could not be resolved at this time.  Try again later.";
                break;

            case EAI_BADFLAGS:
                error_msg = "The flags argument has an invalid value.";
                break;

            case EAI_FAIL:
                error_msg = "A nonrecoverable error occurred.";
                break;

            case EAI_FAMILY:
                error_msg = "The address family was not recognized, or the address length was invalid for the specified family.";
                break;

            case EAI_MEMORY:
                error_msg = "Out of memory.";
                break;

            case EAI_OVERFLOW:
                error_msg = "The buffer pointed to by host or serv was too small.";
                break;

            case EAI_SYSTEM:
                error_msg = get_last_error_message();
                break;
    }
        #endif

        throw std::runtime_error(error_msg);
    }


    //Converts an internet address to a string.
    std::string internet_address_to_string(const internet_address& addr) {
        try {
            return addr.to_string();
        }
        catch (const std::runtime_error& ex) {
            throw std::invalid_argument(ex.what());
        }
    }


    //Converts a string to an internet address.
    internet_address internet_address_from_string(const std::string& str) {
        try {
            return { str.c_str() };
        }
        catch (const std::runtime_error& ex) {
            throw std::invalid_argument(ex.what());
        }
    }


} //namespace netlib
