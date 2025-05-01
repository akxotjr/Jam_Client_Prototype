#pragma once

struct Snapshot
{
	double timestamp = 0.0;
	Vec3 position = {};
	Vec3 velocity = {};
};

class MovementInterpolator
{
public:
	MovementInterpolator();
	~MovementInterpolator() = default;

	void				Update();
	void				AddSnapshot(const Snapshot& snap);

	void				SetInterpolationDelay(double delay) { _interpolationDelay = delay; }
	void				SetExtrapolationLimit(double maxDelta) { _extrapolationLimit = maxDelta; }

	Vec3&				GetLastRenderedPosition() { return _lastRenderedPosition; }

	// temp
	void				SetBasedOnServerRate();

private:
	Vec3				Interpolate(double renderTime);
	Vec3				Extrapolate(double currentTime);
	bool				CanInterpolate(double renderTime);

//	void SetBasedOnServerRate();

	Vec3				Lerp(Vec3& a, Vec3& b, float& t);
	Vec3				CatmullRom(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const double& t);

private:
	USE_LOCK

	Deque<Snapshot>		_snapshotBuffer;
	double				_interpolationDelay = 0.f;
	double				_extrapolationLimit = 0.f;

	Vec3				_lastRenderedPosition = {};
};

