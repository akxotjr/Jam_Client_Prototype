#pragma once

class Scene;

class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

	Vec2& GetPosition() { return _position; }
	void SetPosition(Vec2 position) { _position = position; }

	void SetScene(shared_ptr<Scene> owner) { _owner = owner; }

protected:
	Vec2 _position = {};
	weak_ptr<Scene> _owner;
};

