#pragma once


using UpdateFunc = std::function<void()>;

class TimeManager
{
	DECLARE_SINGLETON(TimeManager)

public:
	void			Init();
	void			Shutdown();
	void			Update();
	void			OnServerTimeReceived(double serverTime);


	uint32			GetFps() { return _fps; }
	double			GetDeltaTime() { return _deltaTime; }
	double			GetAdjustDeltaTime() { return _adjustDeltaTime; }

	void			SetTimeScale(double timeScale) { _timeScale = timeScale; }

	double			GetClientTime() const;

	double			GetRawLocalTime() const;

	double			GetLatestRTT() { return _latestRtt; }
	double			GetAvgRTT() { return _avgRtt; }
	double			GetJitter() { return _jitter; }

	void			SetSession(SessionRef session) { _session = session; }


private:
	USE_LOCK

	UpdateFunc		_update = nullptr;

	uint64			_frequency = 0;
	uint64			_prevCount = 0;
	double			_deltaTime = 0.0;
	double			_adjustDeltaTime = 0.0;
	double			_timeScale = 1.0;
	uint32			_frameCount = 0;
	double			_frameTime = 0.0;
	uint32			_fps = 0;

	double			_sumTime = SYNC_TIME_INTERVAL - 1.0;
	double			_lastTimeSyncSent = 0.0;

	double			_baseServerTime = 0.0;
	double			_baseLocalTime = 0.0;


	double			_latestRtt = 0.0;
	double			_avgRtt = 0.0;
	double			_jitter = 0.0;

	SessionRef		_session = nullptr;
};

