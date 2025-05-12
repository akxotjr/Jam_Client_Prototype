#pragma once

class Utils
{
public:
	template<typename T>
	static T Lerp(T& a, T& b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	static float LerpAngle(float a, float b, float t)
	{
		float diff = fmodf(b - a + PI * 3, PI * 2) - PI;
		return a + diff * t;
	}
};

