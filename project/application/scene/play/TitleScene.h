#pragma once
#include <memory>

#include "2d/Sprite.h"
#include "3d/Object3d.h"
#include "application/Animation/Slide.h"
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
	//デバック用スプライト
	std::unique_ptr<Sprite> sprite_;
	//デバック用オブジェクト
	std::unique_ptr<Object3d> object3d_;
	//デバック用オブジェクト地面
	std::unique_ptr<Object3d> terrain_;
	//キューブの座標
	Vector3 cubePos_ = { 0.0f,0.0f,0.0f };
};

