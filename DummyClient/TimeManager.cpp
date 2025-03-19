#include "pch.h"
#include "TimeManager.h"
#include "SendBuffer.h"
#include "ServerPacketHandler.h"
#include <boost/asio.hpp>

unique_ptr<TimeManager> TimeManager::instance = nullptr;

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_adjustDeltaTime = _deltaTime * _timeScale;
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _deltaTime * _timeScale;

	//_timestamp += _deltaTime;

	if (_timestamp > 0.f)
	{
		int a = 0;
	}

	if (_frameTime >= 1.f)
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);

		_frameTime = 0.f;
		_frameCount = 0;
	}
}