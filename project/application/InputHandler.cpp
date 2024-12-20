#include "InputHandler.h"

#include "input/Input.h"

void InputHandler::AssignMoveRightCommand2PressKeyA()
{
	ICommand* command = new MoveRightCommand();
	pressKeyA_ = command;
}

void InputHandler::AssignMoveLeftCommand2PressKeyD()
{
	ICommand* command = new MoveLeftCommand();
	pressKeyD_ = command;
}

ICommand* InputHandler::HandleInput()
{
	if(Input::GetInstance()->PushKey(DIK_D))
	{
		return pressKeyD_;
	}
	if (Input::GetInstance()->PushKey(DIK_A))
	{
		return pressKeyA_;
	}

	return nullptr;
}
