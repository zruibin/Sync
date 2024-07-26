/*
 *
 * network.cc
 *
 * Created by Ruibin.Chow on 2024/07/26.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "network.h"

#include <iostream>
#include <asio/asio.hpp>

namespace connection {

// https://stackoverflow.com/questions/2674314/get-local-ip-address-using-boost-asio
// https://stackoverflow.com/questions/4716389/c-convert-uint32-ip-address-to-text-x-x-x-x

asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr) {
    asio::ip::address_v6::bytes_type buf;
    memcpy(buf.data(), addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr));
    return asio::ip::make_address_v6(buf, addr->sin6_scope_id);
}

#if defined(_WIN32)

#undef UNICODE
#include <winsock2.h>
// Headers that need to be included after winsock2.h:
#include <iphlpapi.h>
#include <ws2ipdef.h>

typedef IP_ADAPTER_UNICAST_ADDRESS_LH Addr;
typedef IP_ADAPTER_ADDRESSES *AddrList;

std::vector<asio::ip::address> get_local_interfaces() {
    // It's a windows machine, we assume it has 512KB free memory
    DWORD outBufLen = 1 << 19;
    AddrList ifaddrs = (AddrList) new char[outBufLen];

    std::vector<asio::ip::address> res;

    ULONG err = GetAdaptersAddresses(AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, ifaddrs,
        &outBufLen);

    if (err == NO_ERROR) {
        for (AddrList addr = ifaddrs; addr != 0; addr = addr->Next) {
            if (addr->OperStatus != IfOperStatusUp) continue;
            // if (addr->NoMulticast) continue;

            // Find the first IPv4 address
            if (addr->Ipv4Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET) continue;
                    res.push_back(asio::ip::make_address_v4(ntohl(reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
                }
            }

            if (addr->Ipv6Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET6) continue;
                    res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
                }
            }
        }
    } else {

    }
    delete[]((char *)ifaddrs);
    return res;
}

#elif defined(__APPLE__) || defined(__linux__)

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>

std::vector<asio::ip::address> get_local_interfaces() {
    std::vector<asio::ip::address> res;
    ifaddrs *ifs;
    if (getifaddrs(&ifs)) {
        return res;
    }
    for (auto addr = ifs; addr != nullptr; addr = addr->ifa_next) {
        // No address? Skip.
        if (addr->ifa_addr == nullptr) continue;

        // Interface isn't active? Skip.
        if (!(addr->ifa_flags & IFF_UP)) continue;

        if(addr->ifa_addr->sa_family == AF_INET) {
            res.push_back(asio::ip::make_address_v4(ntohl(
                reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
        } else if(addr->ifa_addr->sa_family == AF_INET6) {
            res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
        } else continue;
    }
    freeifaddrs(ifs);
    return res;
}
#else
#error "..."
#endif

void PrintNetworkInfo(void) {
    std::vector<asio::ip::address> addressList = get_local_interfaces();
    std::sort(addressList.begin(), addressList.end(), [](auto& addressA, auto& addressB){
        return addressA.is_v4();
    });
    std::sort(addressList.begin(), addressList.end(), [](auto& addressA, auto& addressB){
        return addressA.is_loopback();
    });
    
    for (auto& address : addressList) {
        std::cout << (address.is_v4() ? "ipv4: " : "ipv6: ") << address.to_string()
            << ", loopback: " << std::boolalpha << address.is_loopback()
            << ", unspecified: " << std::boolalpha << address.is_unspecified()
            << ", multicast: " << std::boolalpha << address.is_multicast()
            << std::endl;
    }
}

}


