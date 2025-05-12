#include "pch.h"
#include "InputManager.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "TimeManager.h"


void InputManager::Init()
{
	_window = Renderer::Instance().GetWindow();
}

void InputManager::Shutdown()
{
	_window = nullptr;
}

void InputManager::Update()
{
	if (!glfwGetWindowAttrib(_window, GLFW_FOCUSED))
		return;

	bool isInGame = SceneManager::Instance().GetIsInGame();
	if (!isInGame) return;

	UpdateKeyField();
	UpdateMouse();
}

Input InputManager::CaptureInput() const
{
	uint32 keyField = GetKeyField();


	if (keyField != _lastSentKeyField || _yaw != _lastYaw || _pitch != _lastPitch)
	{
		double timestamp = TimeManager::Instance().GetClientTime();
		_lastSentKeyField = keyField;
		_lastYaw = _yaw;
		_lastPitch = _pitch;

		return { timestamp, keyField, _yaw, _pitch, 0 };
	}

	return { 0.0, 0, 0.0f, 0.0f, 0 };
}

void InputManager::SetMouseVisible(bool visible)
{
	if (visible)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

bool InputManager::JustPressed(EInputKey key)
{
	int vk = INPUT_KEY_TO_VK.at(key);
	bool isDown = (::GetAsyncKeyState(vk) & 0x8000) != 0;
	bool wasDown = _prevKeyStates[key];
	_prevKeyStates[key] = isDown;
	return (isDown && !wasDown);
}

void InputManager::TogglePause()
{
	_pause = !_pause;
	if (_pause)
		SetMouseVisible(true);
	else
		SetMouseVisible(false);
}

void InputManager::UpdateKeyField()
{
	_keyField = 0;

	for (const auto& [inputKey, vk] : INPUT_KEY_TO_VK)
	{
		if (::GetAsyncKeyState(vk) & 0x8000) // Press 
			_keyField |= (1 << static_cast<uint32>(inputKey));
	}

	if (JustPressed(EInputKey::Pause))
		TogglePause();
}

void InputManager::UpdateMouse()
{
	if (_pause) return;

	double mx, my;
	glfwGetCursorPos(_window, &mx, &my);

	float dx = static_cast<float>(mx - WINDOW_CENTER_X);
	float dy = static_cast<float>(my - WINDOW_CENTER_Y);

	_yaw -= dx * _horizontalSensitivity * DEG2RAD;
	_pitch = std::clamp(_pitch - dy * _verticalSensitivity * DEG2RAD, -PI * 0.5f, PI * 0.5f); 

	glfwSetCursorPos(_window, WINDOW_CENTER_X, WINDOW_CENTER_Y);
}
