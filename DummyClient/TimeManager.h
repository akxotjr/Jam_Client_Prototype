#pragma once

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

private:
	TimeManager() = default;

	TimeManager(const TimeManager&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;

	friend std::unique_ptr<TimeManager> std::make_unique<TimeManager>();

private:
	static unique_ptr<TimeManager> instance;

	uint64	_frequency = 0;
	uint64	_prevCount = 0;
	float	_deltaTime = 0.f;
	float	_adjustDeltaTime = 0.f;
	float	_timeScale = 1.f;

	uint32	_frameCount = 0;
	float	_frameTime = 0.f;
	uint32	_fps = 0;
};

