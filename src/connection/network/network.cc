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

std::string GetPublicIP(const char* localIP) {
    using asio::ip::tcp;
    try {
        asio::io_context io_context;
        
        // 解析目标服务器地址和端口
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("api.ipify.org", "80");
        
        // 创建并打开套接字
        tcp::socket socket(io_context);
        
        // 绑定套接字到特定的本地 IP 地址（网卡）
        asio::ip::address local_address = asio::ip::make_address(localIP);
        socket.open(tcp::v4());
        socket.bind(tcp::endpoint(local_address, 0));
        
        // 连接到目标服务器
        asio::connect(socket, endpoints);
        
        // 构建HTTP请求
        std::string request = "GET / HTTP/1.1\r\n";
        request += "Host: api.ipify.org\r\n";
        request += "Connection: close\r\n\r\n";
        
        // 发送HTTP请求
        asio::write(socket, asio::buffer(request));
        
        // 接收HTTP响应
        asio::streambuf response;
        asio::read_until(socket, response, "\r\n");
        
        // 检查响应状态行
        std::istream response_stream(&response);
        std::string http_version;
        unsigned int status_code;
        std::string status_message;
        response_stream >> http_version >> status_code;
        std::getline(response_stream, status_message);
        
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            std::cout << "Invalid response\n";
            return "";
        }
        
        if (status_code != 200) {
            std::cout << "Response returned with status code " << status_code << "\n";
            return "";
        }
        
        // 读取响应头
        asio::read_until(socket, response, "\r\n\r\n");
        
        // 输出响应头
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
//            std::cout << header << "\n";
        }
        
        // 读取响应体
//        asio::error_code error;
//        while (asio::read(socket, response, asio::transfer_at_least(1), error)) {
////            std::cout << &response;
//        }

        const char* data = asio::buffer_cast<const char*>(response.data());
        std::string result(data, response.size());
        response.consume(response.size());
        
        return result;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return "";
}

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
                    << ", loopback: " << std::boolalpha << address.is_loopback();
//            << ", unspecified: " << std::boolalpha << address.is_unspecified()
//            << ", multicast: " << std::boolalpha << address.is_multicast();
        if (address.is_v4()) {
            std::cout << ", public ip: " << GetPublicIP(address.to_string().c_str()) << std::endl;
        } else {
            std::cout << std::endl;
        }
    }
    
}

}


