#pragma once
#include <memory>

#include "InputHandler.h"
#include "Player.h"

class StageScene
{
public:
	void Init(SpriteCommon* spriteCommon);
	void Update();
	void Draw();

private:
	InputHandler* inputHandler_;
	ICommand* command_;
	std::unique_ptr<Player> player_;
};

