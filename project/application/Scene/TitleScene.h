#pragma once
#include "IScene.h"
#include "3d/Object3d.h"
#include "application/Animation/Slide.h"
#include "application/Manager/BuildingManager.h"


class Object3dCommon;
class SpriteCommon;
class CameraManager;

class TitleScene : public IScene
{
public: // メンバ関数
	TitleScene(CameraManager* cameraManager,SpriteCommon* spriteCommon, Object3dCommon* objectCommon) : cameraManager_(cameraManager),spriteCommon_(spriteCommon),object3dCommon_(objectCommon) {}
	~TitleScene() override;
	void Initialize(SceneManager* sceneManager) override;

	void Update() override;

	void Draw3D() override;

	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

	//ゲーム名のオブジェクトの更新
	void UpdateGameNameOBJ();

private: // メンバ変数
	CameraManager* cameraManager_;
	SpriteCommon* spriteCommon_;
	Object3dCommon* object3dCommon_;

	//スライド
	std::unique_ptr<Slide> slide_;
	//ゲーム名のオブジェクト
	std::unique_ptr<Object3d> gameName_;
	//天球
	std::unique_ptr<Object3d> skyDome_;
	//地面
	std::unique_ptr<Object3d> ground_;
	//ビル
	std::unique_ptr<BuildingManager> buildingManager_;


	//ゲーム名オブジェクトを動かすための変数
	Vector3 initialPosition_;
	float rotationAngle_;
	float moveTimer_;
	float rotationTimer_;
};

