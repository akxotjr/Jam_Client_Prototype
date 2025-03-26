#include "pch.h"
#include "InputManager.h"


shared_ptr<InputManager> InputManager::instance = nullptr;

void InputManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KeyState::None);
}

void InputManager::Update()
{
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{

		if (asciiKeys[key] & 0x80)
		{
			KeyState& state = _states[key];

			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Press;
			else
				state = KeyState::Down;
		}
		else
		{
			KeyState& state = _states[key];


			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}

	// Mouse
	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);
}

Protocol::KeyType InputManager::ConvertToProtoKey(KeyType key)
{
	switch (key)
	{
	case KeyType::W: return Protocol::KeyType::KEY_W;
	case KeyType::A: return Protocol::KeyType::KEY_A;
	case KeyType::S: return Protocol::KeyType::KEY_S;
	case KeyType::D: return Protocol::KeyType::KEY_D;
	case KeyType::Q: return Protocol::KeyType::KEY_Q;
	case KeyType::E: return Protocol::KeyType::KEY_E;

	case KeyType::LeftMouse: return Protocol::KeyType::KEY_LEFT_MOUSE;
	case KeyType::RightMouse: return Protocol::KeyType::KEY_RIGHT_MOUSE;

	case KeyType::SpaceBar: return Protocol::KeyType::KEY_SPACE;

	case KeyType::KEY_1: return Protocol::KeyType::KEY_1;
	case KeyType::KEY_2: return Protocol::KeyType::KEY_2;
		// ...
	default: return Protocol::KeyType::KEY_NONE;
	}
}

KeyType InputManager::ConvertFromProtoKey(Protocol::KeyType key)
{
	switch (key)
	{
	case Protocol::KeyType::KEY_W: return KeyType::W;
	case Protocol::KeyType::KEY_A: return KeyType::A;
	case Protocol::KeyType::KEY_S: return KeyType::S;
	case Protocol::KeyType::KEY_D: return KeyType::D;
	case Protocol::KeyType::KEY_Q: return KeyType::Q;
	case Protocol::KeyType::KEY_E: return KeyType::E;
	case Protocol::KeyType::KEY_LEFT_MOUSE: return KeyType::LeftMouse;
	case Protocol::KeyType::KEY_RIGHT_MOUSE: return KeyType::RightMouse;
	case Protocol::KeyType::KEY_SPACE: return KeyType::SpaceBar;
	case Protocol::KeyType::KEY_1: return KeyType::KEY_1;
	case Protocol::KeyType::KEY_2: return KeyType::KEY_2;
		// ...
	default: return KeyType::None;
	}
}
