#include "pch.h"
#include "TimeManager.h"
#include "SendBuffer.h"
#include "ServerPacketHandler.h"
#include <boost/asio.hpp>

shared_ptr<TimeManager> TimeManager::instance = nullptr;

void TimeManager::Init()
{
	_update = std::bind(&TimeManager::EmptyUpdate, this);

	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
}

void TimeManager::Update()
{
	_update();
}

void TimeManager::OnServerTimeReceived(float serverTime)
{
	{
		WRITE_LOCK
		_rtt = _clientTime - _prevClientTime;
	}

	if (!_isSynchronized)
	{
		_update = std::bind(&TimeManager::RealUpdate, this);
		_isSynchronized = true;
	}
	Synchronize(serverTime, _rtt);
}

void TimeManager::EmptyUpdate()
{
	WRITE_LOCK

	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_adjustDeltaTime = _deltaTime * _timeScale;
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _deltaTime * _timeScale;

	if (_frameTime >= 1.f)
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);

		_frameTime = 0.f;
		_frameCount = 0;
	}
}

void TimeManager::RealUpdate()
{
	WRITE_LOCK

	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_adjustDeltaTime = _deltaTime * _timeScale;
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _deltaTime * _timeScale;

	_clientTime += _deltaTime;
	_sumTime += _deltaTime;

	if (_sumTime >= 5.f) // TODO : change hardcoding
	{
		if (_session)
		{
			Protocol::C_TIMESYNC timesyncPkt;
			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(timesyncPkt);

			_session->Send(sendBuffer);
		}

		_sumTime = 0.f;
	}


	if (_frameTime >= 1.f)
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);

		_frameTime = 0.f;
		_frameCount = 0;
	}
}

void TimeManager::Synchronize(float serverTime, float RTT)
{
	WRITE_LOCK
	SetClientTime(serverTime + RTT / 2.0f);
}
