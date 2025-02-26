#pragma once
#include <memory>

#include "2d/Sprite.h"
#include "3d/Object3d.h"
#include "application/Animation/Slide.h"
#include "application/enemy/EnemyManager.h"
#include "application/enemy/TackleEnemy.h"
#include "engine/scene/interface/BaseScene.h"

class TitleScene : public BaseScene
{
public:
	//初期化
	void Initialize() override;
	//終了
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw3D() override;
	void Draw2D() override;

private: //メンバ変数
	//エネミーマネージャー
	std::unique_ptr<EnemyManager> enemyManager_;
};

