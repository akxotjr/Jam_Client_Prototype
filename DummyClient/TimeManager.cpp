#include "pch.h"
#include "TimeManager.h"
#include "ServerPacketHandler.h"
#include "SceneManager.h"
#include "GameScene.h"


void TimeManager::Init()
{
	_update = [this] { EmptyUpdate(); };

	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
}

void TimeManager::Update()
{
	_update();
}

void TimeManager::OnServerTimeReceived(double serverTime)
{
	double now = GetRawLocalTime();
	_rtt = now - _lastTimeSyncSent;

	_baseServerTime = serverTime + _rtt * 0.5f;
	_baseLocalTime = now;

	if (!_isSynchronized)
	{
		_update = [this] { RealUpdate(); };
		_isSynchronized = true;
	}

	{
		auto gameScene = dynamic_pointer_cast<GameScene>(SceneManager::Instance().GetCurrentScene());
		//if (gameScene)
		//{
		//	auto& actors = gameScene->GetActors();
		//	for (auto& [id, actor] : actors)
		//	{
		//		auto character = dynamic_pointer_cast<Character>(actor);
		//		if (character)
		//		{
		//			character->interpolator.SetBasedOnServerRate();
		//		}
		//	}
		//}
	}

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

void TimeManager::EmptyUpdate()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<double>(_frequency);
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

	_deltaTime = (currentCount - _prevCount) / static_cast<double>(_frequency);
	_adjustDeltaTime = _deltaTime * _timeScale;
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _adjustDeltaTime;

	_sumTime += _deltaTime;

	if (_sumTime >= 5.f) // TODO : change hardcoding
	{
		if (_session)
		{
			//std::cout << "[TCP] Send : C_TIMESYNC\n";
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