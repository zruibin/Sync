/*
 * network.h
 *
 * Created by Ruibin.Chow on 2024/07/26.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <string>

namespace connection {

struct NetworkInfo {
    enum Type {
        Unknow,
        IPV4,
        IPV6
    };
    Type type;
    bool is_v4() const { return type == Type::IPV4; };
    bool is_v6() const { return type == Type::IPV6; };
    
    bool is_loopback() const { return is_loopback_; };
    
    std::string name;
    std::string mac;
    uint16_t mtu;
    
    std::string local;
    uint32_t localAddr;
    
    std::string mask;
    uint32_t maskAddr;
    
    std::string broadcast;
    uint32_t broadcastAddr;
    
    bool is_loopback_;
};

void PrintNetworkInfo(void);

}

#endif /* !NETWORK_H */
