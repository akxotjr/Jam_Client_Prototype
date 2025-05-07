#include "pch.h"
#include "InputManager.h"
#include "Renderer.h"
#include "TimeManager.h"

void InputManager::Init()
{
	_window = Renderer::Instance().GetWindow();
}

void InputManager::Update()
{
	UpdateKeyField();

	double mx, my;
	glfwGetCursorPos(_window, &mx, &my);
	_mousePos = Vec2{ static_cast<float>(mx), static_cast<float>(my) };
}

Input InputManager::CaptureInput() const
{
	if (uint32 keyField = GetKeyField())
	{
		double timestamp = TimeManager::Instance().GetClientTime();
		Vec2 mouse = { static_cast<float>(_mousePos.x), static_cast<float>(_mousePos.y) };

		return { timestamp, keyField, mouse, 0 };
	}

	return { 0.0, 0, Vec2(0,0), 0 };
}

void InputManager::UpdateKeyField()
{
	if (!glfwGetWindowAttrib(_window, GLFW_FOCUSED))
		return;

	_keyField = 0;

	for (const auto& [inputKey, vk] : INPUT_KEY_TO_VK)
	{
		if (::GetAsyncKeyState(vk) & 0x8000) // Press 
			_keyField |= (1 << static_cast<uint32>(inputKey));
	}
}
