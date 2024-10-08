/*
 * socket_interface.h
 *
 * Created by Ruibin.Chow on 2024/08/04.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#include <string>
#include <memory>
#include <functional>

namespace connection {

namespace socket {

class ISocket {
    
public:
    enum class Protocol {
        kUnknow = 0,
        kUDP,
        kKCP,
    };
    
    enum class State {
        kIdle = 0,
        kPrepared,
        kConnecting,
        kConnected,
        kClosing,
        kClosed,
    };
    
    enum class Error {
        kNone = 0,
        kUnknow,
        kMemory,
        kNetwork,
        kCrypto,
        kProtocol,
    };
    
    enum class FrameType {
        kText = 1,
        kBinary,
    };
    
    static const char* ProtocolToString(Protocol protocol);
    static const char* StateToString(State state);
    static const char* ErrorToString(Error error);
    
    using StateChangedHandler = std::function<void(bool connected,
                                                   Protocol protocol,
                                                   const std::string& networkName,
                                                   int networkType)>;
    using FailedHandler = std::function<void(Error error,
                                             const std::string& reason)>;
    using ReceivedFrameHandler = std::function<void(const char* buf,
                                                    std::size_t len,
                                                    FrameType frameType)>;
    
public:
    explicit ISocket() = default;
    virtual ~ISocket() = default;
    
    ISocket(const ISocket&) = delete;
    ISocket& operator=(const ISocket&) = delete;
    
    virtual void SetStateChangedHandler(StateChangedHandler stateChangedHandler) {
        stateChangedHandler = stateChangedHandler_;
    }
    virtual void SetFailedHandler(FailedHandler failedHandler) {
        failedHandler = failedHandler_;
    }
    virtual void SetReceivedFrameHandler(ReceivedFrameHandler receivedFrameHandler) {
        receivedFrameHandler = receivedFrameHandler_;
    }
    
    virtual void SetConnectTimeout(uint32_t timeout) = 0;
    virtual void UseEncrypt(bool encrypt) = 0;
    virtual void Open(const char* ip, uint32_t port) = 0;
    virtual void Connect(const char* ip, uint32_t port) = 0;
    virtual void Close() = 0;
    virtual void Send(const std::string& payload) = 0;
    virtual void Send(const uint8_t* buf, std::size_t len, FrameType frameType) = 0;
    virtual State GetState() = 0;
    virtual Error GetError() = 0;
    virtual bool IsConnected() = 0;
    virtual Protocol GetProtocol() const = 0;
    
protected:
    StateChangedHandler stateChangedHandler_{ nullptr };
    FailedHandler failedHandler_{ nullptr };
    ReceivedFrameHandler receivedFrameHandler_{ nullptr };
};

} // namespace socket

} // namespace connection


#endif /* !SOCKET_INTERFACE_H */
