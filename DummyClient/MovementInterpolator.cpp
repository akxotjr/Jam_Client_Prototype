#include "pch.h"
#include "MovementInterpolator.h"
#include "TimeManager.h"

MovementInterpolator::MovementInterpolator()
	: _interpolationDelay(0.05), _extrapolationLimit(0.25), _lastRenderedPosition(Vec2{0,0})
{
}

void MovementInterpolator::AddSnapshot(const Snapshot& snap)
{
	WRITE_LOCK
	_buffer.push_back(snap);

	while (_buffer.size() > 2 && _buffer.front().timestamp < snap.timestamp - 0.5f)
	{
		_buffer.pop_front();
	}
}

void MovementInterpolator::Update()
{
	double currentTime = TimeManager::GetInstance()->GetClientTime();
	double renderTime = currentTime - _interpolationDelay;

	Vec2 pos = {};
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

void MovementInterpolator::Render()
{
	//
}


Vec2 MovementInterpolator::Interpolate(double renderTime)
{
	READ_LOCK
	for (int i = 0; i < _buffer.size(); i++)
	{
		Snapshot& prev = _buffer[i];	//todo const
		Snapshot& next = _buffer[i + 1];

		if (prev.timestamp <= renderTime && next.timestamp >= renderTime)
		{
			double t = (renderTime - prev.timestamp) / (next.timestamp - prev.timestamp);
			return Lerp(prev.position, next.position, t);
		}
	}

	return _buffer.back().position;
}

Vec2 MovementInterpolator::Extrapolate(double currentTime)
{
	READ_LOCK
	if (_buffer.empty())
	{
		return _lastRenderedPosition;
	}

	Snapshot& last = _buffer.back();	// todo const
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
	if (_buffer.size() < 2)
	{
		return false;
	}

	for (int32 i = 0; i  + 1 < _buffer.size(); i++)
	{
		if (_buffer[i].timestamp <= renderTime && _buffer[i + 1].timestamp >= renderTime)
		{
			return true;
		}
	}
	
	return false;
}

void MovementInterpolator::SetBasedOnServerRate()
{
	double avgRTT = TimeManager::GetInstance()->GetRoundTripTime();
	double jitter = TimeManager::GetInstance()->GetJitter();

	double safetyMargin = 0.03f;

	//_interpolationDelay = 0.05f + avgRTT * 0.5f + jitter + safetyMargin;

	//_interpolationDelay = std::clamp(_interpolationDelay, 0.05f, 0.3f);
}

Vec2 MovementInterpolator::Lerp(Vec2& a, Vec2& b, double& t)
{
	return a * (1.0 - t) + b * t;
}

//Vec2 MovementInterpolator::CatmullRom(Vec2 a, Vec2 b, Vec2 c, Vec2 d, double t)
//{
//	return ((b * 2) + (a * (-1) + c) * t + (a * 2 - b * 5 + c * 4 - d) * t * t + (a * (-1) + b * 3 - c * 3 + d) * t * t * t) * 0.5f;
//}

