#pragma once
#include "application/Animation/Slide.h"
#include "scene/interface/BaseScene.h"

class SpriteCommon;
class CameraManager;

class GameOverScene : public BaseScene
{
public:
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw3D() override;
	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

private:
	//スライド
	std::unique_ptr<Slide> slide_;
	//ゲームオーバー
	std::unique_ptr<Sprite> gameOver_;
};

