#pragma once
#include <cmath>

enum class SceneType
{
	None,
	GameScene
};

struct Vec2
{
	Vec2() {}
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2(POINT pt) : x((float)pt.x), y((float)pt.y) {}

	Vec2 operator+(const Vec2& other)
	{
		Vec2 ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}

	Vec2 operator-(const Vec2& other)
	{
		Vec2 ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}

	Vec2 operator*(float value)
	{
		Vec2 ret;
		ret.x = x * value;
		ret.y = y * value;
		return ret;
	}

	void operator+=(const Vec2& other)
	{
		x += other.x;
		y += other.y;
	}

	void operator-=(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;
	}

	void operator*=(float ratio)
	{
		x *= ratio;
		y *= ratio;
	}

	bool operator==(const Vec2& other) const
	{
		return x == other.x && y == other.y;
	}

	float LengthSquared()
	{
		return x * x + y * y;
	}

	float Length()
	{
		return ::sqrt(LengthSquared());
	}

	void Normalize()
	{
		float length = Length();
		if (length < 0.00000000001f)
			return;

		x /= length;
		y /= length;
	}

	float Dot(Vec2 other)
	{
		return x * other.x + y * other.y;
	}

	float Cross(Vec2 other)
	{
		return x * other.y - y * other.x;
	}

	float x = 0;
	float y = 0;
};

struct Vec2Int
{
	Vec2Int() {}
	Vec2Int(int32 x, int32 y) : x(x), y(y) {}
	Vec2Int(POINT pt) : x(pt.x), y(pt.y) {}

	Vec2Int operator+(const Vec2Int& other)
	{
		Vec2Int ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}

	Vec2Int operator-(const Vec2Int& other)
	{
		Vec2Int ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}

	Vec2Int operator*(int32 value)
	{
		Vec2Int ret;
		ret.x = x * value;
		ret.y = y * value;
		return ret;
	}

	void operator+=(const Vec2Int& other)
	{
		x += other.x;
		y += other.y;
	}

	void operator-=(const Vec2Int& other)
	{
		x -= other.x;
		y -= other.y;
	}

	bool operator==(const Vec2& other)
	{
		return x == other.x && y == other.y;
	}

	int32 LengthSquared()
	{
		return x * x + y * y;
	}

	float Length()
	{
		return (float)::sqrt(LengthSquared());
	}

	int32 Dot(Vec2Int other)
	{
		return x * other.x + y * other.y;
	}

	int32 Cross(Vec2Int other)
	{
		return x * other.y - y * other.x;
	}

	int32 x = 0;
	int32 y = 0;
};


struct Vec3
{
	Vec3() {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	// 연산자 오버로딩
	Vec3 operator+(const Vec3& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	Vec3 operator-(const Vec3& other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	Vec3 operator*(float value) const
	{
		return Vec3(x * value, y * value, z * value);
	}

	void operator+=(const Vec3& other)
	{
		x += other.x; y += other.y; z += other.z;
	}

	void operator-=(const Vec3& other)
	{
		x -= other.x; y -= other.y; z -= other.z;
	}

	void operator*=(float ratio)
	{
		x *= ratio; y *= ratio; z *= ratio;
	}

	bool operator==(const Vec3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	// 벡터 연산
	float LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	float Length() const
	{
		return std::sqrt(LengthSquared());
	}

	void Normalize()
	{
		float len = Length();
		if (len < 1e-8f)
			return;

		x /= len;
		y /= len;
		z /= len;
	}

	float Dot(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	Vec3 Cross(const Vec3& other) const
	{
		return Vec3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	float x = 0;
	float y = 0;
	float z = 0;
};



USING_SHARED_PTR(GameTcpSession);
USING_SHARED_PTR(GameUdpSession);
USING_SHARED_PTR(GameUdpReceiver);
USING_SHARED_PTR(Game)
USING_SHARED_PTR(Actor)
USING_SHARED_PTR(Player)
USING_SHARED_PTR(Scene)
USING_SHARED_PTR(GameScene)
