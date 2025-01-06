#pragma once
#include "IScene.h"

class CameraManager;

class GameOverScene : public IScene
{
public:
	GameOverScene(CameraManager* cameraManager) : cameraManager(cameraManager) {}
	void Initialize(SceneManager* sceneManager) override;
	void Update() override;
	void Draw3D() override;
	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

private:
	//カメラマネージャー
	CameraManager* cameraManager;
};

