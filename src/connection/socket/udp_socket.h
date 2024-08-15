/*
 * udp_socket.h
 *
 * Created by Ruibin.Chow on 2024/08/15.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include "socket_interface.h"

namespace connection {

namespace socket {

class UDPSocket: public ISocket {
    
public:
    void Bind(const char* ip, uint32_t port) override;
    void Open() override;
    void Close() override;
    void Send(const std::string& payload) override;
    void Send(const uint8_t* buf, int len, FrameType frameType) override;
    
    bool IsConnected() override { return state_ == State::kConnected; }
    void SetConnectTimeout(uint32_t timeout) override { timeout_ = timeout; }
    void UseEncrypt(bool encrypt) override { encrypt_ = encrypt; }
    State GetState() override { return state_; }
    Error GetError() override { return error_; }
    Protocol GetProtocol() const override { return Protocol::kUDP; }
    
private:
    uint32_t timeout_;
    bool encrypt_;
    const char* ip_;
    uint32_t port_;
    State state_;
    Error error_;
};

} // namespace socket

} // namespace connection

#endif /* !UDP_SOCKET_H */
