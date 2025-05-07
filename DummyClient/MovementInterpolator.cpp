#include "pch.h"
#include "MovementInterpolator.h"
#include "TimeManager.h"

MovementInterpolator::MovementInterpolator()
	: _interpolationDelay(0.05), _extrapolationLimit(0.25), _renderPosition(Vec3{0,0,0})
{
}

void MovementInterpolator::Process(OUT Vec3& renderPosition, OUT Vec3& renderRotation)
{
	double currentTime = TimeManager::Instance().GetClientTime();
	double renderTime = currentTime - _interpolationDelay;

	if (CanInterpolate(renderTime))
	{
		Interpolate(renderTime);
	}
	else
	{
		Extrapolate(currentTime);
	}

	renderPosition = _renderPosition;
	renderRotation = _renderRotation;
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


void MovementInterpolator::Interpolate(double renderTime)
{
	READ_LOCK
	for (size_t i = 0; i + 1 < _snapshotBuffer.size(); i++)
	{
		Snapshot& prev = _snapshotBuffer[i];
		Snapshot& next = _snapshotBuffer[i + 1];

		if (prev.timestamp <= renderTime && next.timestamp >= renderTime)
		{
			float t = static_cast<float>((renderTime - prev.timestamp) / (next.timestamp - prev.timestamp));
			_renderPosition = Lerp(prev.position, next.position, t);
			_renderRotation = Lerp(prev.rotation, next.rotation, t);
			return;
		}
	}

	_renderPosition = _snapshotBuffer.back().position;
	_renderRotation = _snapshotBuffer.back().rotation;
}

void MovementInterpolator::Extrapolate(double currentTime)
{
	READ_LOCK

	if (_snapshotBuffer.empty())
		return;

	Snapshot& last = _snapshotBuffer.back();	// todo const
	double delta = currentTime - last.timestamp;

	if (delta > _extrapolationLimit)
		return;

	_renderPosition = last.position + last.velocity * static_cast<float>(delta);
	//Vec3 normalizedRotation = last.rotation;
	//normalizedRotation.Normalize();
	//rotation = last.rotation + normalizedRotation * static_cast<float>(delta);

	// TODO : angular velocity 가 필요
	// temp : 일단 그냥 마지막값으로 계속
	//rotation = _lastRenderedRotation;
}

bool MovementInterpolator::CanInterpolate(double renderTime)
{
	WRITE_LOCK	// TODO: ?
	if (_snapshotBuffer.size() < 2)
	{
		return false;
	}

	for (size_t i = 0; i  + 1 < _snapshotBuffer.size(); i++)
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

