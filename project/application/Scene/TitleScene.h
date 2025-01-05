#pragma once
#include "IScene.h"


class CameraManager;

class TitleScene : public IScene
{
public: // メンバ関数
	TitleScene(CameraManager* cameraManager) : cameraManager_(cameraManager) {}

	void Initialize(SceneManager* sceneManager) override;

	void Update() override;

	void Draw3D() override;

	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

private: // メンバ変数
	CameraManager* cameraManager_;
};

