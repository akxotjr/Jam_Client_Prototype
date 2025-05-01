#pragma once

class Scene;

class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor() = default;
	virtual ~Actor();

	virtual void				Init(SceneRef scene);
	virtual void				Update() = 0;
	virtual void				Render(HDC hdc) = 0;


	void						SetActorId(uint32 id) { _actorId = id; }
	uint32						GetActorId() const { return _actorId; }

	SceneRef					GetOwnerScene() const { return _owner.lock(); }

	void						SetTransform(uint64 position, uint64 velocity_speed, uint64 speed);

protected:
	std::weak_ptr<Scene>		_owner;

	uint32						_actorId = 0;

	Vec3						_position = {};
	Vec3						_rotation = {};
	Vec3						_velocity = {};
	float						_moveSpeed = 0.0f;
};

