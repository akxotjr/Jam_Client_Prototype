#include "pch.h"
#include "MovementInterpolator.h"
#include "TimeManager.h"

MovementInterpolator::MovementInterpolator()
	: _interpolationDelay(0.1f), _extrapolationLimit(0.25f), _lastRenderedPosition(Vec2{0,0})
{
}

void MovementInterpolator::AddSnapshot(const Snapshot& snap)
{
	_buffer.push_back(snap);

	while (_buffer.size() > 2 && _buffer.front().timestamp < snap.timestamp - 1.0f)
	{
		_buffer.pop_front();
	}
}

void MovementInterpolator::Update()
{
	// todo : adaptive interpolation delay
	// float avgRTT
	// float jitter
	// float safetyMargin
	// interpolationDelay = avgRTT * 0.5f + jitter +  safetyMargin;
	// interpolationDelay = std::clamp(interpolationDelay, 50ms, 200ms);

	float currentTime = TimeManager::GetInstance()->GetClientTime();
	float renderTime = currentTime - _interpolationDelay;

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


Vec2 MovementInterpolator::Interpolate(float renderTime)
{
	for (int i = 0; i < _buffer.size(); i++)
	{
		Snapshot& prev = _buffer[i];	//todo const
		Snapshot& next = _buffer[i + 1];

		if (prev.timestamp <= renderTime && next.timestamp >= renderTime)
		{
			float t = (renderTime - prev.timestamp) / (next.timestamp - prev.timestamp);
			return Lerp(prev.position, next.position, t);
		}
	}

	return _buffer.back().position;
}

Vec2 MovementInterpolator::Extrapolate(float currentTime)
{
	if (_buffer.empty())
	{
		return _lastRenderedPosition;
	}

	Snapshot& last = _buffer.back();	// todo const
	float delta = currentTime - last.timestamp;

	if (delta > _extrapolationLimit)
	{
		return _lastRenderedPosition;
	}

	return last.position + last.velocity * delta;
}

bool MovementInterpolator::CanInterpolate(float renderTime)
{
	if (_buffer.size() < 2)
	{
		return false;
	}

	for (int32 i = 0; i < _buffer.size(); i++)
	{
		if (_buffer[i].timestamp <= renderTime && _buffer[i + 1].timestamp >= renderTime)
		{
			return true;
		}
	}
	
	return false;
}

Vec2 MovementInterpolator::Lerp(Vec2& a, Vec2& b, float& t)
{
	return a * (1 - t) + b * t;
}

//Vec2 MovementInterpolator::CatmullRom(Vec2 a, Vec2 b, Vec2 c, Vec2 d, float t)
//{
//	return ((b * 2) + (a * (-1) + c) * t + (a * 2 - b * 5 + c * 4 - d) * t * t + (a * (-1) + b * 3 - c * 3 + d) * t * t * t) * 0.5f;
//}

