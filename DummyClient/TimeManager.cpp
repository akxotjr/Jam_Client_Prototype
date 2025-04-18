#include "pch.h"
#include "TimeManager.h"
#include "SendBuffer.h"
#include "ServerPacketHandler.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "Character.h"
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
	float now = GetRawLocalTime();
	_rtt = now - _lastTimeSyncSent;

	_baseServerTime = serverTime + _rtt * 0.5f;
	_baseLocalTime = now;

	if (!_isSynchronized)
	{
		_update = std::bind(&TimeManager::RealUpdate, this);
		_isSynchronized = true;
	}

	{
		auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::GetInstance()->GetCurrentScene());
		if (gameScene)
		{
			auto& actors = gameScene->GetActors();
			for (auto& [id, actor] : actors)
			{
				auto character = dynamic_pointer_cast<Character>(actor);
				if (character)
				{
					character->interpolator.SetBasedOnServerRate();
				}
			}
		}
	}

}

float TimeManager::GetClientTime()
{
	return _baseServerTime + (GetRawLocalTime() - _baseLocalTime);
}

float TimeManager::GetRawLocalTime()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));
	return currentCount / static_cast<float>(_frequency);
}

void TimeManager::EmptyUpdate()
{
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
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_adjustDeltaTime = _deltaTime * _timeScale;
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _adjustDeltaTime;

	_sumTime += _deltaTime;

	if (_sumTime >= 5.f) // TODO : change hardcoding
	{
		if (_session)
		{
			std::cout << "[TCP] Send : C_TIMESYNC\n";
			Protocol::C_TIMESYNC timesyncPkt;
			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBufferTcp(timesyncPkt);
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