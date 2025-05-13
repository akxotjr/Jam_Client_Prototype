#pragma once

class Actor;
class Character;
class Session;

class Scene : public enable_shared_from_this<Scene>
{
public:
	Scene();
	virtual ~Scene();

	virtual void								Init();
	virtual void								Update();
	virtual void								Render();

	void										AddActor(ActorRef actor);
	void										RemoveActor(uint32 id);

	ActorRef									GetActorByActorId(uint32 actorId);

	unordered_map<uint32, ActorRef>&			GetActors() { return _actors; }

protected:
	unordered_map<uint32, ActorRef>				_actors;
};

