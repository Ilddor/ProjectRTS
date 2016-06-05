#pragma once

#include <array>

class InputClass
{
public:
	InputClass();

	void keyDown(unsigned int);
	void keyUp(unsigned int);

	bool isKeyDown(unsigned int);

private:
	std::array<bool, 256> m_keys;
};