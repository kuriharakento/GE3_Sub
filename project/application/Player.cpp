#include "Player.h"

void Player::Init(SpriteCommon* spriteCommon,const std::string& filePath)
{
	sprite = std::make_unique<Sprite>();
	sprite->Initialize(spriteCommon, filePath);

	pos_ = Vector2(300.0f, 300.0f);
	sprite->SetPosition(pos_);
}

void Player::Update()
{
	sprite->SetPosition(pos_);
	sprite->Update();
}

void Player::Draw()
{
	sprite->Draw();
}

void Player::MoveRight()
{
	pos_.x -= 3.0f;
}

void Player::MoveLeft()
{
	pos_.x += 3.0f;
}
