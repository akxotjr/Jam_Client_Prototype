#include "pch.h"
#include "Floor.h"
#include "Renderer.h"

void Floor::Init(SceneRef scene)
{
	Super::Init(scene);
}

void Floor::Update()
{
}

void Floor::Render()
{
	Renderer::Instance().DrawPlane(glm::vec3(_position.x, _position.y, _position.z), glm::vec2(WORLD_RANGE_MAX * 2, WORLD_RANGE_MAX * 2), _color);
	Renderer::Instance().DrawGrid(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
}
