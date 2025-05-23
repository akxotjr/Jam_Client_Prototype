#include "pch.h"
#include "TimeManager.h"
#include "ServerPacketHandler.h"
#include <cmath>


void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
}

void TimeManager::Shutdown()
{
	_update = nullptr;
	_session = nullptr;
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<double>(_frequency);
	_adjustDeltaTime = _deltaTime * _timeScale;
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _adjustDeltaTime;

	_sumTime += _deltaTime;

	if (_sumTime >= SYNC_TIME_INTERVAL)
	{
		if (_session)
		{
			//std::cout << "[TCP] Send : C_SYNC_TIME\n";
			Protocol::C_SYNC_TIME timeSyncPkt;
			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBufferTcp(timeSyncPkt);
			_session->Send(sendBuffer);

			_lastTimeSyncSent = GetRawLocalTime();
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

void TimeManager::OnServerTimeReceived(double serverTime)
{
	double now = GetRawLocalTime();
	_latestRtt = now - _lastTimeSyncSent;

	_avgRtt = std::lerp(_avgRtt, _latestRtt, 0.1);
	_jitter = std::abs(_latestRtt - _avgRtt);

	_baseServerTime = serverTime - _latestRtt * 0.5f - LOGIC_TICK_INTERVAL;
	_baseLocalTime = now;
}

double TimeManager::GetClientTime() const
{
	return _baseServerTime + (GetRawLocalTime() - _baseLocalTime);
}

double TimeManager::GetRawLocalTime() const
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));
	return currentCount / static_cast<double>(_frequency);
}