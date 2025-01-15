#include "GameScene.h"
#include "engine/scene/manager/SceneManager.h"
#include "2d/SpriteCommon.h"

void GameScene::Initialize()
{
	// プレイヤーの初期化
	player_ = std::make_unique<Player>();
	player_->Initialize("player.obj", sceneManager_->GetObject3dCommon(), sceneManager_->GetSpriteCommon(), sceneManager_->GetCameraManager());

	// 敵の初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(sceneManager_->GetObject3dCommon(), sceneManager_->GetCameraManager(), player_.get(), "enemy.obj");

	// 建物の初期化
	buildingManager_ = std::make_unique<BuildingManager>();
	buildingManager_->Initialize(sceneManager_->GetObject3dCommon(), sceneManager_->GetCameraManager());
	// 建物の生成
	buildingManager_->GenerateBuilding(60, 10.0f,200.0f);

	// 当たり判定マネージャーの初期化
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	// スライドの初期化
	slide_ = std::make_unique<Slide>();
	slide_->Initialize(sceneManager_->GetSpriteCommon());

	//スライドの最初の状態を設定
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);

	// 天球の初期化
	skyDome_ = std::make_unique<Object3d>();
	skyDome_->Initialize(sceneManager_->GetObject3dCommon());
	skyDome_->SetModel("skydome.obj");
	skyDome_->SetScale({ 0.5f,0.5f,0.5f });

	// 地面の初期化
	ground_ = std::make_unique<Object3d>();
	ground_->Initialize(sceneManager_->GetObject3dCommon());
	ground_->SetModel("ground.obj");
}

void GameScene::Finalize()
{
	player_.reset();
	enemyManager_.reset();
	buildingManager_.reset();
	collisionManager_.reset();
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
		// プレイヤーの更新
		player_->Update();

		// 敵の更新
		enemyManager_->Update();

		// 建物の更新
		buildingManager_->Update();

		// 当たり判定の更新
		AddCollisions(collisionManager_.get(), player_.get(), enemyManager_.get(), buildingManager_.get());
		collisionManager_->Update();
		if(enemyManager_->IsEmpty())
		{
			ChangePhase(ScenePhase::End);
			isClear_ = true;
		}
		if (!player_->IsAlive())
		{
			ChangePhase(ScenePhase::End);
			isGameOver_ = true;
		}
		break;
	case ScenePhase::End:
		slide_->Update();
		if(slide_->IsFinish())
		{
			if (isClear_)
			{
				sceneManager_->ChangeScene("CLEAR");
			} else if (isGameOver_)
			{
				sceneManager_->ChangeScene("GAMEOVER");
			}
		}
		break;
	}

	ground_->Update(sceneManager_->GetCameraManager());
	skyDome_->Update(sceneManager_->GetCameraManager());
}

void GameScene::Draw3D()
{
	//プレイヤーの描画
	player_->Draw();

	//敵の描画
	enemyManager_->Draw();

	//建物の描画
	buildingManager_->Draw();

	//地面の描画
	ground_->Draw();

	//天球の描画
	skyDome_->Draw();
}

void GameScene::Draw2D()
{
	//プレイヤーのUIの描画
	player_->DrawUI();
	//スライドの描画
	slide_->Draw();
}

void GameScene::OnPhaseChanged(ScenePhase newPhase)
{
	switch (newPhase)
	{
	case ScenePhase::Start:
		slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);
		break;
	case ScenePhase::Play:
		enemyManager_->SpawnEnemies(10, 15.0f);
		break;
	case ScenePhase::End:
		slide_->Start(Slide::Status::SlideInFromBothSides, 1.0f);
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
