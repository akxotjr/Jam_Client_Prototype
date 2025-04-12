#pragma once

enum class KeyType
{
	LeftMouse = VK_LBUTTON,
	RightMouse = VK_RBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	SpaceBar = VK_SPACE,

	KEY_1 = '1',
	KEY_2 = '2',

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',
	Q = 'Q',
	E = 'E',

	None
};

enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX) + 1,
	KEY_STATE_COUNT = static_cast<int32>(KeyState::End)
};

class InputManager : public enable_shared_from_this<InputManager>
{
	DECLARE_SINGLETON(InputManager)

public:
	void				Init(HWND hwnd);
	void				Update();

	KeyState			GetButtonState(KeyType key) { return GetState(key); }
	bool				GetButton(KeyType key) { return GetState(key) == KeyState::Press; }
	bool				GetButtonDown(KeyType key) { return GetState(key) == KeyState::Down; }
	bool				GetButtonUp(KeyType key) { return GetState(key) == KeyState::Up; }

	POINT				GetMousePos() { return _mousePos; }

	Protocol::KeyType	ConvertToProtoKey(KeyType key);
	KeyType				ConvertFromProtoKey(Protocol::KeyType key);

private:
	KeyState			GetState(KeyType key) { return _states[static_cast<uint8>(key)]; }

private:
	HWND				_hwnd = 0;
	vector<KeyState>	_states;
	POINT				_mousePos;
};
