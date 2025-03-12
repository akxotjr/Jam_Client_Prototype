#pragma once

class Actor;
class Character;

class Scene : public enable_shared_from_this<Scene>
{
public:
	Scene();
	~Scene();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

	Character* GetPlayer() { return _player.get(); }

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

private:
	shared_ptr<Character> _player = nullptr;
	vector<shared_ptr<Actor>> _otherActors;
};

