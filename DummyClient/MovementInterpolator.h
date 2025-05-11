#pragma once

struct Snapshot
{
	double timestamp = 0.0;
	Vec3 position = {};
	Vec3 velocity = {};
	Vec3 rotation = {};
};

class MovementInterpolator
{
public:
	MovementInterpolator();
	~MovementInterpolator() = default;

	void				Process(Vec3& renderPosition, Vec3& renderRotation);
	void				AddSnapshot(const Snapshot& snap);

	void				SetInterpolationDelay(double delay) { _interpolationDelay = delay; }
	void				SetExtrapolationLimit(double maxDelta) { _extrapolationLimit = maxDelta; }

	Vec3&				GetLastRenderedPosition() { return _renderPosition; }

	// temp
	void				SetBasedOnServerTime();

private:
	bool				Interpolate(double renderTime);
	void				Extrapolate(double currentTime);
	//bool				CanInterpolate(double renderTime);

	Vec3				Lerp(const Vec3& a, const Vec3& b, const float t);
	Vec3				CatmullRom(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const double& t);

private:
	USE_LOCK

	Deque<Snapshot>		_snapshotBuffer;

	double				_interpolationDelay = 0.0f;
	double				_extrapolationLimit = 0.0f;

	Vec3				_renderPosition = {};
	Vec3				_renderRotation = {};
};

