#pragma once

//enum class KeyType
//{
//	W = 'W',
//	S = 'S',
//	A = 'A',
//	D = 'D',
//	SpaceBar = VK_SPACE,
//	LeftMouse = VK_LBUTTON,
//	RightMouse = VK_RBUTTON,
//	KEY_1 = '1',
//	KEY_2 = '2',
//	KEY_3 = '3',
//	KEY_4 = '4',
//	Q = 'Q',
//	E = 'E',
//	TabOpen = VK_TAB,
//
//	None
//};

enum class EInputKey : uint32
{
	MoveForward = 0,    // W
	MoveBackward = 1,   // S
	MoveLeft = 2,       // A
	MoveRight = 3,      // D
	Jump = 4,           // Space
	Fire = 5,           // Left mouse
	AltFire = 6,        // Right mouse
	Skill1 = 7,         // 1
	Skill2 = 8,         // 2
	Skill3 = 9,         // 3
	Skill4 = 10,        // 4
	QSkill = 11,        // Q
	ESkill = 12,        // E
	TabOpen = 13,       // Tab
};

static const std::unordered_map<EInputKey, int32> INPUT_KEY_TO_VK = {
	{ EInputKey::MoveForward, 'W' },
	{ EInputKey::MoveBackward, 'S' },
	{ EInputKey::MoveLeft, 'A' },
	{ EInputKey::MoveRight, 'D' },
	{ EInputKey::Jump, VK_SPACE },
	{ EInputKey::Fire, VK_LBUTTON },
	{ EInputKey::AltFire, VK_RBUTTON },
	{ EInputKey::Skill1, '1' },
	{ EInputKey::Skill2, '2' },
	{ EInputKey::Skill3, '3' },
	{ EInputKey::Skill4, '4' },
	{ EInputKey::QSkill, 'Q' },
	{ EInputKey::ESkill, 'E' },
	{ EInputKey::TabOpen, VK_TAB }
};


enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

//enum
//{
//	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX) + 1,
//	KEY_STATE_COUNT = static_cast<int32>(KeyState::End)
//};

struct Input
{
	Input(double ts, uint32 kf, Vec2 pos, int seq, double dt)
		: timestamp(ts), keyField(kf), mousePosition(pos), sequence(seq), deltaTime(dt) {}

	double	timestamp = 0.0;
	uint32	keyField = 0;
	Vec2	mousePosition = {};
	uint32	sequence = 0;
	double	deltaTime = 0.0;
};

class InputManager
{
	DECLARE_SINGLETON(InputManager)

public:
	void				Init(/*HWND hwnd*/);
	void				Update();

	//KeyState			GetButtonState(KeyType key) const { return GetState(key); }
	//bool				GetButton(KeyType key) const { return GetState(key) == KeyState::Press; }
	//bool				GetButtonDown(KeyType key) const { return GetState(key) == KeyState::Down; }
	//bool				GetButtonUp(KeyType key) const { return GetState(key) == KeyState::Up; }

	//POINT				GetMousePos() const { return _mousePos; }

	Input				CaptureInput() const;

	uint32				GetKeyField() const { return _keyField; }

private:
	void				UpdateKeyField();
	//KeyState			GetState(KeyType key) const { return _states[static_cast<uint8>(key)]; }

private:
	//HWND				_hwnd = nullptr;
	GLFWwindow*			_window = nullptr;

	uint32				_keyField = 0;
	uint32				_keyDownField = 0;
	uint32				_keyUpField = 0;

	Vec2				_mousePos;
};
