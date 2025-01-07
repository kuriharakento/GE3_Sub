#pragma once
#include <memory>

#include "IScene.h"
#include "application/Animation/Slide.h"
#include "application/Entities/Player.h"
#include "application/Manager/BuildingManager.h"
#include "application/Manager/CollisionManager.h"
#include "application/Manager/EnemyManager.h"

class SpriteCommon;
class CameraManager;
class Object3dCommon;

class GameScene : public IScene
{
public: // メンバ関数
	~GameScene() override;
	GameScene(Object3dCommon* objectCommon, CameraManager* camera, SpriteCommon* spriteCommon) : object3dCommon_(objectCommon), cameraManager_(camera), spriteCommon_(spriteCommon) {}

	void Initialize(SceneManager* sceneManager) override;

	void Update() override;

	void Draw3D() override;

	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

	void AddCollisions(CollisionManager* collisionManager, Player* player, EnemyManager* enemyManager, BuildingManager* buildingManager);

private: //ポインタ
	Object3dCommon* object3dCommon_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;

private: // メンバ変数
	/*===[ プレイヤー ]===*/
	std::unique_ptr<Player> player_;
	/*===[ 敵 ]===*/
	std::unique_ptr<EnemyManager> enemyManager_;
	/*===[ 建物 ]===*/
	std::unique_ptr<BuildingManager> buildingManager_;
	/*===[ 当たり判定マネージャー ]===*/
	std::unique_ptr<CollisionManager> collisionManager_;
	/*===[スライド]===*/
	std::unique_ptr<Slide> slide_;
	/*===[ 天球 ]===*/
	std::unique_ptr<Object3d> skyDome_;
	/*===[ 地面 ]===*/
	std::unique_ptr<Object3d> ground_;
};

