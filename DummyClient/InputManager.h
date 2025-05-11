#pragma once

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
	Pause = 14,			// ESC
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
	{ EInputKey::TabOpen, VK_TAB },
	{ EInputKey::Pause, VK_ESCAPE },
};


struct Input
{
	Input(double ts, uint32 kf, int seq) : timestamp(ts), keyField(kf), sequence(seq) {}

	double	timestamp = 0.0;
	uint32	keyField = 0;
	uint32	sequence = 0;
};

class InputManager
{
	DECLARE_SINGLETON(InputManager)

public:
	void				Init();
	void				Shutdown();

	void				Update();

	Input				CaptureInput() const;

	uint32				GetKeyField() const { return _keyField; }
	Vec2&				GetMousePosition() { return _mousePos; }

	float				GetYaw() { return _yaw; }
	float				GetPitch() { return _pitch; }

	void				SetMouseVisible(bool visible);

	bool				JustPressed(EInputKey key);
	void				TogglePause();

private:
	void				UpdateKeyField();
	void				UpdateMouse();

private:
	GLFWwindow*			_window = nullptr;

	bool				_pause = false;

	uint32				_keyField = 0;
	uint32				_keyDownField = 0;
	uint32				_keyUpField = 0;

	std::unordered_map<EInputKey, bool> _prevKeyStates;


	Vec2				_mousePos;
	Vec2				_lastMousePos;
	float				_horizontalSensitivity = 0.001f;
	float				_verticalSensitivity = 0.001f;
	float				_yaw = 0.0f;
	float				_pitch = 0.0f;
};
