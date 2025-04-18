#pragma once

class UdpReceiver
{
    enum { BUFFER_SIZE = 0x10000 };

public:
    UdpReceiver(boost::asio::any_io_executor executor);
    virtual ~UdpReceiver();

    bool                        Start(ServiceRef service);
    virtual void                OnRecv(SessionRef& session, BYTE* buffer, int32 len) = 0;

private:
    void                        RegisterRecv();
    bool                        ProcessRecv(int32 numOfBytes);
    int32                       IsParsingPacket(BYTE* buffer, const int32 len, ReliableUdpSessionRef session);

private:
    udp::socket                     _socket;
    udp::endpoint                   _remoteEndpoint;
    //boost::asio::any_io_executor    _executor;

    RecvBuffer                      _recvBuffer;
    SOCKADDR_IN                     _remoteAddr = {};	// is thread safe? 

    std::weak_ptr<Service>          _service;
};

