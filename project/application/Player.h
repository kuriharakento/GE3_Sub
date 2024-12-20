#pragma once
#include <memory>

#include "2d/Sprite.h"

class Player
{
public:
	void Init(SpriteCommon* spriteCommon, const std::string& filePath);
	void Update();
	void Draw();

	void MoveRight();
	void MoveLeft();

private:
	std::unique_ptr<Sprite> sprite;
	Vector2 pos_;

};

