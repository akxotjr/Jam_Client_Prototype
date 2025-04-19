#pragma once
#include "UdpReceiver.h"

class GameUdpReceiver : public UdpReceiver
{
public:
	GameUdpReceiver(boost::asio::any_io_executor executor);
	virtual ~GameUdpReceiver();

	/* UdpReceiver impl */
	void OnRecv(SessionRef& session, BYTE* buffer, int32 len) override;
};

