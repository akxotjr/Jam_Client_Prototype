#pragma once

struct Snapshot
{
	float timestamp = 0.f;
	Vec2 position = {};
	Vec2 velocity = {};
};

class MovementInterpolator
{
public:
	MovementInterpolator();

	void AddSnapshot(const Snapshot& snap);
	void Update();
	void Render();

	void SetInterpolationDelay(float delay) { _interpolationDelay = delay; }
	void SetExtrapolationLimit(float maxDelta) { _extrapolationLimit = maxDelta; }

	Vec2& GetLastRenderedPosition() { return _lastRenderedPosition; }

	// temp
	void SetBasedOnServerRate();

private:
	Vec2 Interpolate(float renderTime);
	Vec2 Extrapolate(float currentTime);
	bool CanInterpolate(float renderTime);

//	void SetBasedOnServerRate();

	Vec2 Lerp(Vec2& a, Vec2& b, float& t);
	Vec2 CatmullRom(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, const float& t);

private:
	USE_LOCK

	Deque<Snapshot> _buffer;
	float _interpolationDelay = 0.f;
	float _extrapolationLimit = 0.f;

	Vec2 _lastRenderedPosition = {};
};

