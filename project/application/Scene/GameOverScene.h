#pragma once
#include "IScene.h"
#include "application/Animation/Slide.h"

class SpriteCommon;
class CameraManager;

class GameOverScene : public IScene
{
public:
	~GameOverScene() override;
	GameOverScene(CameraManager* cameraManager, SpriteCommon* spriteCommon) : cameraManager(cameraManager),spriteCommon_(spriteCommon) {}
	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw3D() override;
	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

private:
	//カメラマネージャー
	CameraManager* cameraManager;
	//スプライト共通
	SpriteCommon* spriteCommon_;

	//スライド
	std::unique_ptr<Slide> slide_;
};

