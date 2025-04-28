#pragma once

struct Snapshot
{
	double timestamp = 0.f;
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

	void SetInterpolationDelay(double delay) { _interpolationDelay = delay; }
	void SetExtrapolationLimit(double maxDelta) { _extrapolationLimit = maxDelta; }

	Vec2& GetLastRenderedPosition() { return _lastRenderedPosition; }

	// temp
	void SetBasedOnServerRate();

private:
	Vec2 Interpolate(double renderTime);
	Vec2 Extrapolate(double currentTime);
	bool CanInterpolate(double renderTime);

//	void SetBasedOnServerRate();

	Vec2 Lerp(Vec2& a, Vec2& b, double& t);
	Vec2 CatmullRom(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, const double& t);

private:
	USE_LOCK

	Deque<Snapshot> _buffer;
	double _interpolationDelay = 0.f;
	double _extrapolationLimit = 0.f;

	Vec2 _lastRenderedPosition = {};
};

