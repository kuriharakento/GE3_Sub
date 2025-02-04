#pragma once
#include <memory>

#include "application/Animation/Slide.h"
#include "application/Entities/Player.h"
#include "application/Manager/BuildingManager.h"
#include "application/Manager/CollisionManager.h"
#include "application/Manager/EnemyManager.h"
#include "scene/interface/BaseScene.h"

class GameScene : public BaseScene
{
public: // メンバ関数
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw3D() override;

	void Draw2D() override;

private: // メンバ関数
	void OnPhaseChanged(ScenePhase newPhase) override;

	void AddCollisions(CollisionManager* collisionManager, Player* player, EnemyManager* enemyManager, BuildingManager* buildingManager);

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
	/*===[ ゲームクリア条件 ]===*/
	std::unique_ptr<Sprite> gameClear_;

	//クリア条件
	bool isClear_ = false;
	//ゲームオーバー条件
	bool isGameOver_ = false;
};

