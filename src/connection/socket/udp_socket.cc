/*
 *
 * udp_socket.cc
 *
 * Created by Ruibin.Chow on 2024/08/15.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "udp_socket.h"
#include <iostream>
#include <string_view>

namespace connection {

namespace socket {

void UDPSocket::Open(const char* ip, uint32_t port) {
    udp::endpoint localPoint;
    std::string_view ipStr{ip};
    if (ipStr.length() == 0) {
        localPoint = udp::endpoint(asio::ip::make_address(ip), port);
    } else {
        localPoint = udp::endpoint(udp::v4(), port);
    }
    
    asio::error_code ec;
    socket_.open(localPoint.protocol(), ec);
    if (ec) {
        std::cout << "Error opening socket: " << ec.message() << std::endl;
        return "";
    }

    socket_.bind(localPoint, ec);
    if (ec) {
        std::cout << "Error binding socket: " << ec.message() << std::endl;
        return "";
    }
}

void UDPSocket::Connect(const char* ip, uint32_t port) {
    remotePoint_ = udp::endpoint(asio::ip::address::from_string(ip), port);
    socket_.connect(remotePoint_);
    if (receivedFrameHandler_) {
        socket_.async_receive_from(asio::buffer(data_, max_length),
                                   remotePoint_,
                                   [this](std::error_code ec, std::size_t bytes_recvd)
                                   {
            if (!ec && bytes_recvd > 0 && strlen(data_) > 0) {
                receivedFrameHandler_(data_, bytes_recvd, FrameType::kText);
            }
        });
    }
    io_service_.run();
}

void UDPSocket::Close() {
    if (socket_.is_open()) {
        socket_.close();
    }
    if (!io_service_.stopped()) {
        io_service_.stop();
    }
}

void UDPSocket::Send(const std::string& payload) {
    if (payload.length() > 0) {
        Send(reinterpret_cast<const uint8_t*>(payload.data()), payload.length(), FrameType::kText);
    }
}

void UDPSocket::Send(const uint8_t* buf, std::size_t len, FrameType frameType) {
    if (len <= 0) {
        return;
    }
    asio::error_code ec;
    socket_.send_to(asio::buffer(buf, len), remotePoint_, 0, ec);
    if (ec) {
        std::cout << "Send error: " << ec.message();
    }
}

} // namespace socket

} // namespace connection

