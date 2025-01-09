#pragma once
#include "application/scene/interface/IScene.h"

class TitleScene : public IScene
{
public:
	//初期化
	void Initialize(SceneManager* sceneManager) override;
	//終了
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw3D() override;
	void Draw2D() override;
};

