#pragma once
#include "Command.h"


class InputHandler
{
public:
	ICommand* HandleInput();

	void AssignMoveRightCommand2PressKeyA();
	void AssignMoveLeftCommand2PressKeyD();

private:
	ICommand* pressKeyA_;
	ICommand* pressKeyD_;
};

