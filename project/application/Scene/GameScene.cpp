#include "GameScene.h"
#include "SceneManager.h"
#include "2d/SpriteCommon.h"
#include "input/Input.h"

GameScene::~GameScene()
{
	player_.reset();
	enemyManager_.reset();
	buildingManager_.reset();
	collisionManager_.reset();
}

void GameScene::Initialize(SceneManager* sceneManager)
{
	// シーンマネージャーの設定
	sceneManager_ = sceneManager;

	// プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize("Building.obj", object3dCommon_, cameraManager_);

	// 敵の初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(object3dCommon_, cameraManager_, player_.get(), "Building.obj");

	// 建物の初期化
	buildingManager_ = std::make_unique<BuildingManager>();
	buildingManager_->Initialize(object3dCommon_, cameraManager_);

	// 当たり判定マネージャーの初期化
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	// スライドの初期化
	slide_ = std::make_unique<Slide>();
	slide_->Initialize(spriteCommon_);

	//スライドの最初の状態を設定
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.25f);
}

void GameScene::Update()
{
	switch (currentPhase_)
	{
	case ScenePhase::Start:
		slide_->Update();
		if (slide_->IsFinish())
		{
			ChangePhase(ScenePhase::Play);
		}
		break;
	case ScenePhase::Play:
		// カメラの更新
		cameraManager_->Update();

		// プレイヤーの更新
		player_->Update();

		// 敵の更新
		enemyManager_->Update();

		// 建物の更新
		buildingManager_->Update();

		// 当たり判定の更新
		AddCollisions(collisionManager_.get(), player_.get(), enemyManager_.get(), buildingManager_.get());
		collisionManager_->Update();
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))
		{
			ChangePhase(ScenePhase::End);
		}
		break;
	case ScenePhase::End:
		slide_->Update();
		if (slide_->IsFinish()) 
		{
			sceneManager_->ChangeScene("GameClearScene");
		}
		break;
	}
}

void GameScene::Draw3D()
{
	//プレイヤーの描画
	player_->Draw();

	//敵の描画
	enemyManager_->Draw();

	//建物の描画
	buildingManager_->Draw();
}

void GameScene::Draw2D()
{
	slide_->Draw();
}

void GameScene::OnPhaseChanged(ScenePhase newPhase)
{
	switch (newPhase)
	{
	case ScenePhase::Start:
		slide_->Start(Slide::Status::SlideOutFromBothSides, 1.25f);
		break;
	case ScenePhase::Play:

		break;
	case ScenePhase::End:
		slide_->Start(Slide::Status::SlideInFromBothSides, 1.25f);
		break;
	}
}

void GameScene::AddCollisions(CollisionManager* collisionManager, Player* player, EnemyManager* enemyManager, BuildingManager* buildingManager)
{
		collisionManager->Clear();
		//プレイヤーの当たり判定を追加
		collisionManager->AddCollidable(player);
		//プレイヤーの弾の当たり判定を追加
		for (int i = 0; i < player->GetMachineGun()->GetBullets().size(); i++)
		{
			collisionManager->AddCollidable(player->GetMachineGun()->GetBullet(i));
		}
		//敵の当たり判定を追加
		ICollidable* enemyCollidable;
		for (int i = 0; i < enemyManager->GetEnemies().size(); i++)
		{
			enemyCollidable = enemyManager->GetEnemy(i);
			collisionManager->AddCollidable(enemyCollidable);

			//ミサイルの当たり判定を追加
			for (int j = 0; j < enemyManager->GetEnemy(i)->GetMissiles().size(); j++)
			{
				collisionManager->AddCollidable(enemyManager->GetEnemy(i)->GetMissile(j));
			}
		}
		//建物の当たり判定を追加
		for (int i = 0; i < buildingManager->GetBuildings().size(); i++)
		{
			collisionManager->AddCollidable(buildingManager->GetBuilding(i));
		}
}
