#include "pch.h"
#include "MovementInterpolator.h"
#include "TimeManager.h"
#include <cmath>

MovementInterpolator::MovementInterpolator()
	: _interpolationDelay(0.05), _extrapolationLimit(0.25), _renderPosition(Vec3{0,0,0})
{
}

void MovementInterpolator::Process(OUT Vec3& renderPosition, OUT Vec3& renderRotation)
{
	double currentTime = TimeManager::Instance().GetClientTime();
	double renderTime = currentTime - _interpolationDelay;

	if (!Interpolate(renderTime))
	{
		Extrapolate(currentTime);
	}

	renderPosition = _renderPosition;
	renderRotation = _renderRotation;
}

void MovementInterpolator::AddSnapshot(const Snapshot& snap)
{
	WRITE_LOCK
	//_snapshotBuffer.push_back(snap);

	//if (_snapshotBuffer.size() > 2 && _snapshotBuffer.front().timestamp < snap.timestamp - _interpolationDelay)
	//{
	//	_snapshotBuffer.pop_front();
	//}

	auto it = std::upper_bound(
		_snapshotBuffer.begin(), _snapshotBuffer.end(),
		snap.timestamp,
		[](double t, const Snapshot& s) { return t < s.timestamp; }
	);

	_snapshotBuffer.insert(it, snap);

	// 오래된 snapshot 제거
	double cutoff = snap.timestamp - 0.5f;
	auto eraseIt = std::find_if(_snapshotBuffer.begin(), _snapshotBuffer.end(),
		[cutoff](const Snapshot& s) { return s.timestamp >= cutoff; });

	_snapshotBuffer.erase(_snapshotBuffer.begin(), eraseIt);
}


bool MovementInterpolator::Interpolate(double renderTime)
{
	//READ_LOCK
	//for (size_t i = 0; i + 1 < _snapshotBuffer.size(); i++)
	//{
	//	Snapshot& prev = _snapshotBuffer[i];
	//	Snapshot& next = _snapshotBuffer[i + 1];

	//	if (prev.timestamp <= renderTime && next.timestamp >= renderTime)
	//	{
	//		float t = static_cast<float>((renderTime - prev.timestamp) / (next.timestamp - prev.timestamp));
	//		_renderPosition = Lerp(prev.position, next.position, t);
	//		_renderRotation = Lerp(prev.rotation, next.rotation, t);
	//		return;
	//	}
	//}

	//_renderPosition = _snapshotBuffer.back().position;
	//_renderRotation = _snapshotBuffer.back().rotation;

	READ_LOCK

	if (_snapshotBuffer.size() < 2)
		return false;

	auto comp = [](const Snapshot& s, double t) {
		return s.timestamp < t;
		};

	auto it = std::lower_bound(_snapshotBuffer.begin(), _snapshotBuffer.end(), renderTime, comp);

	if (it == _snapshotBuffer.begin() || it == _snapshotBuffer.end())
	{
		return false;
	}

	const Snapshot& next = *it;
	const Snapshot& prev = *(it - 1);

	float t = static_cast<float>((renderTime - prev.timestamp) / (next.timestamp - prev.timestamp));
	_renderPosition = Lerp(prev.position, next.position, t);
	_renderRotation = Lerp(prev.rotation, next.rotation, t);
	return true;
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

void MovementInterpolator::SetBasedOnServerTime()
{
	double avgRTT = TimeManager::Instance().GetAvgRTT();
	double jitter = TimeManager::Instance().GetJitter();

	WRITE_LOCK

	_interpolationDelay = LOGIC_TICK_INTERVAL + avgRTT * 0.5 + jitter;
	_interpolationDelay = std::clamp(_interpolationDelay, MIN_INTERPOLATION_DELAY, MAX_INTERPOLATION_DELAY);
}

Vec3 MovementInterpolator::Lerp(const Vec3& a, const Vec3& b, const float t)
{
	return a * (1.0f - t) + b * t;
}

//Vec2 MovementInterpolator::CatmullRom(Vec2 a, Vec2 b, Vec2 c, Vec2 d, double t)
//{
//	return ((b * 2) + (a * (-1) + c) * t + (a * 2 - b * 5 + c * 4 - d) * t * t + (a * (-1) + b * 3 - c * 3 + d) * t * t * t) * 0.5f;
//}

