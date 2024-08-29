/*
 * udp_socket.h
 *
 * Created by Ruibin.Chow on 2024/08/15.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include "socket_interface.h"
#include <asio/asio.hpp>

namespace connection {

namespace socket {

using namespace asio::ip;

class UDPSocket: public ISocket {
    
public:
    void Open(const char* ip, uint32_t port) override;
    void Connect(const char* ip, uint32_t port) override;
    void Close() override;
    void Send(const std::string& payload) override;
    void Send(const uint8_t* buf, std::size_t len, FrameType frameType) override;
    
    bool IsConnected() override { return state_ == State::kConnected; }
    void SetConnectTimeout(uint32_t timeout) override { timeout_ = timeout; }
    void UseEncrypt(bool encrypt) override { encrypt_ = encrypt; }
    State GetState() override { return state_; }
    Error GetError() override { return error_; }
    Protocol GetProtocol() const override { return Protocol::kUDP; }
    
private:
    uint32_t timeout_;
    bool encrypt_;
    State state_;
    Error error_;
    
private:
    asio::io_service io_service_;
    udp::socket socket_{io_service_};
    udp::endpoint remotePoint_;
    enum { max_length = 4096 }; //长度需要关注
    char data_[max_length];
};

} // namespace socket

} // namespace connection

#endif /* !UDP_SOCKET_H */
