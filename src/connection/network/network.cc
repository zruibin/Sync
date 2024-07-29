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

#if defined(_WIN32)

#undef UNICODE
#include <winsock2.h>
// Headers that need to be included after winsock2.h:
#include <iphlpapi.h>
#include <ws2ipdef.h>

typedef IP_ADAPTER_UNICAST_ADDRESS_LH Addr;
typedef IP_ADAPTER_ADDRESSES *AddrList;

#elif defined(__APPLE__) || defined(__linux__)

#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
//#include <net/if.h>
//#include <net/if_dl.h>

#endif

// https://stackoverflow.com/questions/2674314/get-local-ip-address-using-boost-asio
// https://stackoverflow.com/questions/4716389/c-convert-uint32-ip-address-to-text-x-x-x-x
// https://segmentfault.com/a/1190000005138358
// https://www.cnblogs.com/fnlingnzb-learner/p/5942150.html

asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr) {
    asio::ip::address_v6::bytes_type buf;
    memcpy(buf.data(), addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr));
    return asio::ip::make_address_v6(buf, addr->sin6_scope_id);
}

#if defined(__APPLE__) || defined(__linux__)
uint16_t get_mtu(const char *interface) {
    int fd;
    struct ifreq ifr;
 
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFMTU, &ifr)) {
        perror("ioctl");
        return -1;
    }
    close(fd);
    return ifr.ifr_mtu;
}

//std::string get_mac_address(const char* interface) {
//    ifaddrs *ifs;
//    if (getifaddrs(&ifs)) {
//        return "";
//    }
//    char format_str[20] = { 0 };
//    for (auto addr = ifs; addr != nullptr; addr = addr->ifa_next) {
//        if (addr->ifa_addr == nullptr) continue;
//        if (!(addr->ifa_flags & IFF_UP)) continue;
//
//        if (addr->ifa_addr->sa_family == AF_LINK &&
//            strcmp(addr->ifa_name, interface) == 0) {
//            struct sockaddr_dl* sdl = (struct sockaddr_dl*)addr->ifa_addr;
//            unsigned char* mac = (unsigned char*)LLADDR(sdl);
////            printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
////                               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//            snprintf(format_str,
//                     sizeof(format_str) - 1,
//                     "%02x:%02x:%02x:%02x:%02x:%02x",
//                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//        }
//    }
//    freeifaddrs(ifs);
//    return format_str;
//}
#endif

NetworkInfo parse_info_ipv4(
#if defined(_WIN32)
AddrList addr
#elif defined(__APPLE__) || defined(__linux__)
const ifaddrs *addr
#endif
) {
    uint32_t localAddr = reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr;
    asio::ip::address local = asio::ip::make_address_v4(ntohl(localAddr));
    uint32_t maskAddr = reinterpret_cast<sockaddr_in *>(addr->ifa_netmask)->sin_addr.s_addr;
    asio::ip::address mask = asio::ip::make_address_v4(ntohl(maskAddr));
    uint32_t broadcastAddr = reinterpret_cast<sockaddr_in *>(addr->ifa_dstaddr)->sin_addr.s_addr;
    asio::ip::address broadcast = asio::ip::make_address_v4(ntohl(broadcastAddr));
    
    NetworkInfo info;
    info.name = addr->ifa_name;
    info.local = local.to_string();
    info.localAddr = localAddr;
    info.mask = mask.to_string();
    info.maskAddr = maskAddr;
    info.broadcast = broadcast.to_string();
    info.broadcastAddr = broadcastAddr;
    info.is_loopback_ = local.is_loopback();
    info.type = NetworkInfo::Type::IPV4;

#if defined(__APPLE__) || defined(__linux__)
    info.mtu = get_mtu(addr->ifa_name);
#endif
    
    return info;
}

NetworkInfo parse_info_ipv6(
#if defined(_WIN32)
AddrList addr
#elif defined(__APPLE__) || defined(__linux__)
const ifaddrs *addr
#endif
) {
    asio::ip::address local = sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr));
    NetworkInfo info;
    info.name = addr->ifa_name;
    info.local = local.to_string();
    info.is_loopback_ = local.is_loopback();
    info.type = NetworkInfo::Type::IPV6;
    return info;
}


#if defined(_WIN32)

std::vector<NetworkInfo> get_local_interfaces() {
    // It's a windows machine, we assume it has 512KB free memory
    DWORD outBufLen = 1 << 19;
    AddrList ifaddrs = (AddrList) new char[outBufLen];

    std::vector<NetworkInfo> res;

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
                    res.emplace_back(parse_info_ipv4(addr));
                }
            }

            if (addr->Ipv6Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET6) continue;
                    res.emplace_back(parse_info_ipv6(addr));
                }
            }
        }
    } else {

    }
    delete[]((char *)ifaddrs);
    return res;
}

#elif defined(__APPLE__) || defined(__linux__)

std::vector<NetworkInfo> get_local_interfaces() {
    std::vector<NetworkInfo> res;
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
            res.emplace_back(parse_info_ipv4(addr));
        } else if(addr->ifa_addr->sa_family == AF_INET6) {
            res.emplace_back(parse_info_ipv6(addr));
        } else continue;
    }
    freeifaddrs(ifs);
    return res;
}
#else
#error "..."
#endif

std::string GetPublicIP(const char* localIP) {
    asio::io_service io_service;
    asio::ip::udp::socket socket(io_service);
    
    asio::ip::address local_address = asio::ip::make_address(localIP);
    asio::ip::udp::endpoint localPoint(local_address, 0);
    
    asio::error_code ec;
    socket.open(localPoint.protocol(), ec);
    if (ec) {
        std::cout << "Error opening socket: " << ec.message() << std::endl;
        return "";
    }
    
    socket.bind(localPoint, ec);
    if (ec) {
        std::cout << "Error binding socket: " << ec.message() << std::endl;
        return "";
    }
 
    asio::ip::udp::endpoint serverPoint(asio::ip::address::from_string("42.194.251.114"), 5566);
 
    std::string message = "ip";
    socket.send_to(asio::buffer(message), serverPoint, 0, ec);
    if (ec) {
        std::cout << "Send error: " << ec.message();
        return "";
    }

    char buf[0xFF];
    memset(buf, 0, 0xFF);
    socket.receive_from(asio::buffer(buf), serverPoint);
    
    return buf;
}

void PrintNetworkInfo(void) {
    std::vector<NetworkInfo> networks = get_local_interfaces();
    std::sort(networks.begin(), networks.end(), [](auto& networkA, auto& networkB){
        return networkA.is_v4();
    });

    for (auto& network : networks) {
        std::cout << (network.is_v4() ? "ipv4: " : "ipv6: ") << network.local;
        std::cout << ", name: " << network.name;
        std::cout << ", loopback: " << std::boolalpha << network.is_loopback();
        if (network.is_v4()) {
//            std::cout << ", public ip: " << GetPublicIP(network.local.c_str());
            std::cout << ", mtu: " << network.mtu;
        }
        std::cout << std::endl;
    }
}

}


