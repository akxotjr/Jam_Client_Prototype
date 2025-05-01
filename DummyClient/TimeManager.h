#pragma once


using UpdateFunc = std::function<void()>;

class TimeManager
{
	DECLARE_SINGLETON(TimeManager)

public:
	void			Init();
	void			Update();
	void			OnServerTimeReceived(double serverTime);


	uint32			GetFps() { return _fps; }
	double			GetDeltaTime() { return _deltaTime; }
	double			GetAdjustDeltaTime() { return _adjustDeltaTime; }

	void			SetTimeScale(double timeScale) { _timeScale = timeScale; }

	double			GetClientTime();

	double			GetRawLocalTime();

	double			GetRoundTripTime() { return _rtt; }
	double			GetJitter() { return _jitter; }

	void			SetSession(SessionRef session) { _session = session; }

private:
	void			EmptyUpdate();
	void			RealUpdate();

private:
	USE_LOCK

	UpdateFunc		_update = nullptr;

	uint64			_frequency = 0;
	uint64			_prevCount = 0;
	double			_deltaTime = 0.f;
	double			_adjustDeltaTime = 0.f;
	double			_timeScale = 1.f;
	uint32			_frameCount = 0;
	double			_frameTime = 0.f;
	uint32			_fps = 0;

	//float			_clientTime = 0.f;
	//float			_prevClientTime = 0.f;	
	double			_sumTime = 0.f;
	double			_lastTimeSyncSent = 0.f;

	double			_baseServerTime = 0.f;
	double			_baseLocalTime = 0.f;


	double			_rtt = 0.f;
	double			_jitter = 0.f;

	bool			_isSynchronized = false;

	SessionRef		_session = nullptr;
};

