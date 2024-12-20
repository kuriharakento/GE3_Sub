#include "StageScene.h"

void StageScene::Init(SpriteCommon* spriteCommon)
{
	inputHandler_ = new InputHandler();

	//Assign commands
	inputHandler_->AssignMoveRightCommand2PressKeyA();
	inputHandler_->AssignMoveLeftCommand2PressKeyD();

	//player
	player_ = std::make_unique<Player>();
	player_->Init(spriteCommon, "./Resources/uvChecker.png");

}

void StageScene::Update()
{
	command_ = inputHandler_->HandleInput();

	if (command_ != nullptr)
	{
		command_->Execute(*player_.get());
	}

	player_->Update();
}

void StageScene::Draw()
{
	player_->Draw();
}
