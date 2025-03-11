#pragma once

class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

	Vec2& GetPosition() { return _position; }
	void SetPosition(Vec2 position) { _position = position; }

	float& GetTimestamp() { return _timestamp; }


private:
	Vec2 _position = {};
	float _timestamp = 0.f;
};

