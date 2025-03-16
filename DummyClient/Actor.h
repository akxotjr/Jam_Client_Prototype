#pragma once

class Scene;

class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	virtual void Init(shared_ptr<Scene> owner);
	virtual void Update();
	virtual void Render(HDC hdc);

	Vec2& GetPosition() { return _position; }
	void SetPosition(Vec2 position) { _position = position; }

	//float& GetTimestamp() { return _timestamp; }


protected:
	Vec2 _position = {};
	//float _timestamp = 0.f;

	weak_ptr<Scene> _owner;
};

