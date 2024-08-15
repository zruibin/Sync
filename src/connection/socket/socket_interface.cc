/*
 *
 * socket_interface.cc
 *
 * Created by Ruibin.Chow on 2024/08/04.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "socket_interface.h"

namespace connection {

namespace socket {

const int kConnnectTotalTimeout = 60 * 1000;
const int kConnnectTimeout = 10 * 1000;
const int kConnnectShortTimeout = 5 * 1000;
const int kCloseTimeout = 2 * 1000;
const int kReconnectInterval = 1 * 1000;
const int kHeartbeatPingInterval = 15 * 1000;
const int kPongTimeout = 20 * 1000;
const int kBadRTT = 500;

const char* ISocket::ProtocolToString(Protocol protocol) {
    switch (protocol) {
        case Protocol::kUDP:
            return "UDP";
        case Protocol::kKCP:
            return "KCP";
        default:
            return "";
    }
}

const char* ISocket::StateToString(State state) {
    switch (state) {
        case State::kIdle:
            return "Idle";
        case State::kPrepared:
            return "Prepared";
        case State::kConnecting:
            return "Connecting";
        case State::kConnected:
            return "Connected";
        case State::kClosing:
            return "Closing";
        case State::kClosed:
            return "Closed";
        default:
            return "";
    }
}

const char* ISocket::ErrorToString(Error error) {
    switch (error) {
        case Error::kNone:
            return "None";
        case Error::kUnknow:
            return "Unknow";
        case Error::kMemory:
            return "Memory";
        case Error::kNetwork:
            return "Network";
        case Error::kCrypto:
            return "Crypto";
        case Error::kProtocol:
            return "Protocol";
        default:
            return "";
    }
}

} // namespace socket

} // namespace connection
