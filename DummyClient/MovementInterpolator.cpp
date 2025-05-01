#include "pch.h"
#include "MovementInterpolator.h"
#include "TimeManager.h"

MovementInterpolator::MovementInterpolator()
	: _interpolationDelay(0.05), _extrapolationLimit(0.25), _lastRenderedPosition(Vec3{0,0,0})
{
}

void MovementInterpolator::Update()
{
	double currentTime = TimeManager::Instance().GetClientTime();
	double renderTime = currentTime - _interpolationDelay;

	Vec3 pos = {};
	if (CanInterpolate(renderTime))
	{
		pos = Interpolate(renderTime);
	}
	else
	{
		pos = Extrapolate(currentTime);
	}

	_lastRenderedPosition = pos;
}

void MovementInterpolator::AddSnapshot(const Snapshot& snap)
{
	WRITE_LOCK
	_snapshotBuffer.push_back(snap);

	while (_snapshotBuffer.size() > 2 && _snapshotBuffer.front().timestamp < snap.timestamp - 0.5f)
	{
		_snapshotBuffer.pop_front();
	}
}


Vec3 MovementInterpolator::Interpolate(double renderTime)
{
	READ_LOCK
	for (int i = 0; i < _snapshotBuffer.size(); i++)
	{
		Snapshot& prev = _snapshotBuffer[i];	//todo const
		Snapshot& next = _snapshotBuffer[i + 1];

		if (prev.timestamp <= renderTime && next.timestamp >= renderTime)
		{
			float t = static_cast<float>((renderTime - prev.timestamp) / (next.timestamp - prev.timestamp));
			return Lerp(prev.position, next.position, t);
		}
	}

	return _snapshotBuffer.back().position;
}

Vec3 MovementInterpolator::Extrapolate(double currentTime)
{
	READ_LOCK
	if (_snapshotBuffer.empty())
	{
		return _lastRenderedPosition;
	}

	Snapshot& last = _snapshotBuffer.back();	// todo const
	double delta = currentTime - last.timestamp;

	if (delta > _extrapolationLimit)
	{
		return _lastRenderedPosition;
	}

	return last.position + last.velocity * delta;
}

bool MovementInterpolator::CanInterpolate(double renderTime)
{
	WRITE_LOCK
	if (_snapshotBuffer.size() < 2)
	{
		return false;
	}

	for (int32 i = 0; i  + 1 < _snapshotBuffer.size(); i++)
	{
		if (_snapshotBuffer[i].timestamp <= renderTime && _snapshotBuffer[i + 1].timestamp >= renderTime)
		{
			return true;
		}
	}
	
	return false;
}

void MovementInterpolator::SetBasedOnServerRate()
{
	double avgRTT = TimeManager::Instance().GetRoundTripTime();
	double jitter = TimeManager::Instance().GetJitter();

	double safetyMargin = 0.03f;

	//_interpolationDelay = 0.05f + avgRTT * 0.5f + jitter + safetyMargin;

	//_interpolationDelay = std::clamp(_interpolationDelay, 0.05f, 0.3f);
}

Vec3 MovementInterpolator::Lerp(Vec3& a, Vec3& b, float& t)
{
	return a * (1.0f - t) + b * t;
}

//Vec2 MovementInterpolator::CatmullRom(Vec2 a, Vec2 b, Vec2 c, Vec2 d, double t)
//{
//	return ((b * 2) + (a * (-1) + c) * t + (a * 2 - b * 5 + c * 4 - d) * t * t + (a * (-1) + b * 3 - c * 3 + d) * t * t * t) * 0.5f;
//}

