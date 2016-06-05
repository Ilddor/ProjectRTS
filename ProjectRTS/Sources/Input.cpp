#include "Input.h"


InputClass::InputClass()
{
    std::fill(m_keys.begin(), m_keys.end(), false);
}

void InputClass::keyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
}


void InputClass::keyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
}


bool InputClass::isKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}