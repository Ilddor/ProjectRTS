#pragma once

#include <array>

class InputClass
{
public:
	InputClass();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	std::array<bool, 256> m_keys;
};