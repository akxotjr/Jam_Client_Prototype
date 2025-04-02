#pragma once


using UpdateFunc = std::function<void()>;

class TimeManager : public enable_shared_from_this<TimeManager>
{
	DECLARE_SINGLETON(TimeManager)

public:
	void	Init();
	void	Update();
	void	OnServerTimeReceived(float serverTime);


	uint32	GetFps() { return _fps; }
	float	GetDeltaTime() { return _deltaTime; }
	float	GetAdjustDeltaTime() { return _adjustDeltaTime; }

	void	SetTimeScale(float timeScale) { _timeScale = timeScale; }

	//float	GetClientTime() { return _clientTime; }
	float	GetClientTime();
	//void	SetClientTime(float clientTime) { _clientTime = clientTime; }

	float	GetRawLocalTime();

	float	GetRoundTripTime() { return _rtt; }
	float	GetJitter() { return _jitter; }

	//void	SetPrevClientTime() { _prevClientTime = _clientTime; }

	void	SetSession(SessionRef session) { _session = session; }


private:
	void EmptyUpdate();
	void RealUpdate();

	//void Synchronize(float serverTime, float RTT);


private:
	USE_LOCK

	UpdateFunc		_update = nullptr;

	uint64			_frequency = 0;
	uint64			_prevCount = 0;
	float			_deltaTime = 0.f;
	float			_adjustDeltaTime = 0.f;
	float			_timeScale = 1.f;
	uint32			_frameCount = 0;
	float			_frameTime = 0.f;
	uint32			_fps = 0;

	//float			_clientTime = 0.f;
	//float			_prevClientTime = 0.f;	
	float			_sumTime = 0.f;
	float			_lastTimeSyncSent = 0.f;

	float			_baseServerTime = 0.f;
	float			_baseLocalTime = 0.f;


	float			_rtt = 0.f;
	float			_jitter = 0.f;

	bool			_isSynchronized = false;

	SessionRef		_session = nullptr;
};

