#pragma once

class Utils
{
public:
	static void PrintText(HDC hdc, Vec2 pos, const wstring& str);
	static void DrawCircle(HDC hdc, Vec2 pos, int32 radius);
};

