#include "ObstacleManager.h"

#include "application/GameObject/component/collision/OBBColliderComponent.h"
#include "jsonEditor/JsonEditorManager.h"

void ObstacleManager::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	// ポインタをメンバ変数に記録
	object3dCommon_ = object3dCommon;
	lightManager_ = lightManager;

	// リストの初期化
	obstacles_.clear();

	// JSONエディタに登録
	obstacleData_ = std::make_shared<ObstacleData>();
	JsonEditorManager::GetInstance()->Register("obstacles", obstacleData_);
}

void ObstacleManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Obstacle Manager");
	
	ImGui::End();
#endif
	if(obstacleData_->GetObstacleCount() != obstacles_.size())
	{
		// 障害物の数が異なる場合は再生成
		CreateObstacles("cube.obj");
	}
	

	for (auto& obstacle : obstacles_)
	{
		if (obstacle)
		{
			obstacle->Update();
		}
	}
}

void ObstacleManager::Draw(CameraManager* camera)
{
	for (auto& obstacle : obstacles_)
	{
		if (obstacle)
		{
			obstacle->Draw(camera);
		}
	}
}

void ObstacleManager::LoadObstacleData(const std::string& jsonName)
{
	obstacleData_->Initialize(jsonName);
}

void ObstacleManager::CreateObstacles(const std::string& modelName)
{
    // 既存の障害物をクリア
    obstacles_.clear();

    // 位置、回転、スケールの情報を取得
	auto& transforms = obstacleData_->GetObstacles();


    // 障害物を生成
	for (uint32_t i = 0; i < obstacleData_->GetObstacleCount(); ++i)
    {
        auto obstacle = std::make_unique<Obstacle>("obstacle");
        obstacle->Initialize(object3dCommon_,lightManager_);
        obstacle->SetModel(modelName);
		obstacle->SetPosition(transforms[i].translate);
		obstacle->SetRotation(transforms[i].rotate);
		obstacle->SetScale(transforms[i].scale);

		// 衝突判定コンポーネントを追加
		obstacle->AddComponent("OBBCollider", std::make_unique<OBBColliderComponent>(obstacle.get()));
        obstacles_.push_back(std::move(obstacle));
    }
}

void ObstacleManager::ApplyObstacleData()
{
	// 障害物の位置、回転、スケールをデータから適用
	for (size_t i = 0; i < obstacles_.size(); ++i)
	{
		
	}
}

