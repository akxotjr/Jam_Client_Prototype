#pragma once


using UpdateFunc = std::function<void()>;

class TimeManager
{
public:
	~TimeManager() = default;

	static TimeManager* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = make_unique<TimeManager>();
		}
		return instance.get();
	}

	void Init();
	void Update();

	uint32 GetFps() { return _fps; }
	float GetDeltaTime() { return _deltaTime; }
	float GetAdjustDeltaTime() { return _adjustDeltaTime; }

	void SetTimeScale(float timeScale) { _timeScale = timeScale; }

	float GetClientTime() { return _clientTime; }
	void SetClientTime(float clientTime) { _clientTime = clientTime; }

	void SetPrevClientTime() { _prevClientTime = _clientTime; }

	void SetSession(SessionRef session) { _session = session; }

	void OnServerTimeReceived(float serverTime);

	//void SetOffset(float offset) { _offset = offset; }



private:
	void EmptyUpdate();
	void RealUpdate();

	void Synchronize(float serverTime, float RTT);

private:
	TimeManager() = default;

	TimeManager(const TimeManager&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;

	friend std::unique_ptr<TimeManager> std::make_unique<TimeManager>();

private:
	USE_LOCK

	static unique_ptr<TimeManager> instance;

	UpdateFunc	_update = nullptr;

	uint64		_frequency = 0;
	uint64		_prevCount = 0;
	float		_deltaTime = 0.f;
	float		_adjustDeltaTime = 0.f;
	float		_timeScale = 1.f;

	float		_clientTime = 0.f;
	float		_prevClientTime = 0.f;
	float		_sumTime = 0.f;
	float		_rtt = 0.f;

	Atomic<bool> _isSynchronized = false;

	uint32		_frameCount = 0;
	float		_frameTime = 0.f;
	uint32		_fps = 0;

	SessionRef	_session = nullptr;
};

