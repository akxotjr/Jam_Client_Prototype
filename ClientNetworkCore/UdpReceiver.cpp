#include "pch.h"
#include "UdpReceiver.h"
#include "Service.h"

UdpReceiver::UdpReceiver(boost::asio::any_io_executor executor)
    : _socket(executor), _recvBuffer(BUFFER_SIZE)
{
}

UdpReceiver::~UdpReceiver()
{
    _socket.close();
}

bool UdpReceiver::Start(ServiceRef service)
{
    _service = service;
    if (_service.lock() == nullptr)
        return false;

    _socket.open(udp::v4());
    _socket.set_option(boost::asio::socket_base::reuse_address(true));
    _socket.bind(udp::endpoint(udp::v4(), 0));

    //udp::resolver resolver(_socket.get_executor());
    //auto endpoints = resolver.resolve(_service.lock()->GetUdpNetAddress().ip, _service.lock()->GetUdpNetAddress().port);
    //_endpoint = *endpoints.begin();

    //boost::asio::socket_base::reuse_address option(true);
    //_socket.set_option(option);

    //_socket.bind(_endpoint);

    const int32 maxSessionCount = _service.lock()->GetMaxUdpSessionCount();
    for (int32 i = 0; i < maxSessionCount; i++)
    {
        RegisterRecv();
    }

    return true;
}

void UdpReceiver::RegisterRecv()
{
    if (_socket.is_open() == false)
        return;

    _socket.async_receive_from(
        boost::asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()), _remoteEndpoint,
        [this](const boost::system::error_code& ec, size_t bytes_transferred) mutable
        {
            if (!ec && bytes_transferred > 0)
            {
                ProcessRecv(bytes_transferred);
            }
            else
            {
                std::cout << "[ERROR] UDP fail to receive : " << ec.message() << " (code=" << ec.value() << ")" << std::endl;
            }
        }
    );
}

bool UdpReceiver::ProcessRecv(int32 numOfBytes)
{
    auto session = _service.lock()->FindOrCreateUdpSession(_remoteEndpoint);

    if (_recvBuffer.OnWrite(numOfBytes) == false)
    {
        return false;
    }

    int32 dataSize = _recvBuffer.DataSize();
    int32 processLen = IsParsingPacket(_recvBuffer.ReadPos(), dataSize, session);

    if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
    {
        return false;
    }

    _recvBuffer.Clean();

    RegisterRecv();
    return true;
}

int32 UdpReceiver::IsParsingPacket(BYTE* buffer, const int32 len, ReliableUdpSessionRef session)
{
    int32 processLen = 0;

    while (true)
    {
        int32 dataSize = len - processLen;

        if (dataSize < sizeof(UdpPacketHeader))
            break;

        UdpPacketHeader* header = reinterpret_cast<UdpPacketHeader*>(&buffer[processLen]);

        if (dataSize < header->size || header->size < sizeof(UdpPacketHeader))
            break;

        auto baseSession = static_pointer_cast<Session>(session);
        OnRecv(baseSession, &buffer[processLen], header->size);

        processLen += header->size;
    }

    return processLen;
}