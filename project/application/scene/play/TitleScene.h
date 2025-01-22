#pragma once
#include "3d/Object3d.h"
#include "application/Animation/Slide.h"
#include "application/Manager/BuildingManager.h"
#include "scene/interface/BaseScene.h"

class TitleScene : public  BaseScene
{
public: // メンバ関数
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw3D() override;

	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

	//ゲーム名のオブジェクトの更新
	void UpdateGameNameOBJ();

private: // メンバ変数
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
	//デバック用スプライト
	std::unique_ptr<Sprite> debugSprite_;

	//ゲーム名オブジェクトを動かすための変数
	Vector3 initialPosition_;
	float rotationAngle_;
	float moveTimer_;
	float rotationTimer_;
};

