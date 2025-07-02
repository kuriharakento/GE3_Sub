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
	obstacleData_ = std::make_unique<ObstacleData>();
	obstacleData_->Initialize(jsonName);
}

void ObstacleManager::CreateObstacles(const std::string& modelName)
{
    // 既存の障害物をクリア
    obstacles_.clear();

    // 位置、回転、スケールの情報を取得
    const auto& positions = obstacleData_->GetPositions();
    const auto& rotations = obstacleData_->GetRotations();
    const auto& scales = obstacleData_->GetScales();

    // 最小サイズを取得（配列のサイズに不一致がある場合のために）
    size_t count = positions.size();
    count = std::min(count, rotations.size());
    count = std::min(count, scales.size());

    // 障害物を生成
    for (size_t i = 0; i < count; ++i)
    {
        auto obstacle = std::make_unique<Obstacle>("obstacle");
        obstacle->Initialize(object3dCommon_,lightManager_);
        obstacle->SetModel(modelName);
		obstacle->SetPosition(positions[i]);
		obstacle->SetRotation(rotations[i]);
		obstacle->SetScale(scales[i]);

		// 衝突判定コンポーネントを追加
		obstacle->AddComponent("OBBCollider", std::make_unique<OBBColliderComponent>(obstacle.get()));
        obstacles_.push_back(std::move(obstacle));
    }
}

