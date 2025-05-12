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

	auto it = std::upper_bound(
		_snapshotBuffer.begin(), _snapshotBuffer.end(),
		snap.timestamp,
		[](double t, const Snapshot& s) { return t < s.timestamp; }
	);

	_snapshotBuffer.insert(it, snap);

	double cutoff = snap.timestamp - 0.5f;
	auto eraseIt = std::find_if(_snapshotBuffer.begin(), _snapshotBuffer.end(),
		[cutoff](const Snapshot& s) { return s.timestamp >= cutoff; });

	_snapshotBuffer.erase(_snapshotBuffer.begin(), eraseIt);
}


bool MovementInterpolator::Interpolate(double renderTime)
{
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
	_renderPosition = Utils::Lerp(prev.position, next.position, t);
	_renderRotation = Utils::Lerp(prev.rotation, next.rotation, t);
	return true;
}

void MovementInterpolator::Extrapolate(double currentTime)
{
	READ_LOCK

	if (_snapshotBuffer.empty())
		return;

	Snapshot& last = _snapshotBuffer.back();
	double delta = currentTime - last.timestamp;

	if (delta > _extrapolationLimit)
		return;

	_renderPosition = last.position + last.velocity * static_cast<float>(delta);
}

void MovementInterpolator::SetBasedOnServerTime()
{
	double avgRTT = TimeManager::Instance().GetAvgRTT();
	double jitter = TimeManager::Instance().GetJitter();

	WRITE_LOCK

	_interpolationDelay = LOGIC_TICK_INTERVAL + avgRTT * 0.5 + jitter;
	_interpolationDelay = std::clamp(_interpolationDelay, MIN_INTERPOLATION_DELAY, MAX_INTERPOLATION_DELAY);
}

