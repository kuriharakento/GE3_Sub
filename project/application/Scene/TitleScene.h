#pragma once
#include "IScene.h"
#include "application/Animation/Slide.h"


class SpriteCommon;
class CameraManager;

class TitleScene : public IScene
{
public: // メンバ関数
	TitleScene(CameraManager* cameraManager,SpriteCommon* spriteCommon) : cameraManager_(cameraManager),spriteCommon_(spriteCommon) {}

	void Initialize(SceneManager* sceneManager) override;

	void Update() override;

	void Draw3D() override;

	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

private: // メンバ変数
	CameraManager* cameraManager_;
	SpriteCommon* spriteCommon_;

	//スライド
	std::unique_ptr<Slide> slide_;
};

