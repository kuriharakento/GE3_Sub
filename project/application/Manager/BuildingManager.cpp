#include "BuildingManager.h"

#include <random>
#include <unordered_set>

#include "3d/ModelManager.h"

void BuildingManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera)
{
	//引数で受け取ったポインタを記録
	objectCommon_ = objectCommon;
	cameraManager_ = camera;

	//モデルの読み込み
	ModelManager::GetInstance()->LoadModel(filePath_);

	//リストの初期化
	buildings_.clear();

	//建物を生成
	GenerateBuilding(10);
}

void BuildingManager::Update()
{
	//全ての建物を更新
	for (auto& building : buildings_)
	{
		building->Update(cameraManager_);
	}
}

void BuildingManager::Draw()
{
	//全ての建物を描画
	for (auto& building : buildings_)
	{
		building->Draw();
	}
}

void BuildingManager::GenerateBuilding(int count)
{
    // 乱数生成器の初期化
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-50.0f, 50.0f); // 位置の範囲
    std::uniform_real_distribution<float> scaleDist(0.5f, 2.0f);  // Y軸の大きさの範囲

    // 配置済みの位置を記録するセット
    std::unordered_set<std::string> occupiedPositions;

    // 建物を生成
    for (int i = 0; i < count; i++)
    {
        Vector3 position;
        std::string posKey;
        do
        {
            position = { posDist(gen), 0.0f, posDist(gen) };
            posKey = std::to_string(static_cast<int>(position.x)) + "_" + std::to_string(static_cast<int>(position.z));
        } while (occupiedPositions.find(posKey) != occupiedPositions.end());

        // 位置を記録
        occupiedPositions.insert(posKey);

        // 建物の生成と初期化
        auto building = std::make_unique<Building>();
        building->Initialize(filePath_, objectCommon_);

        // 位置とスケールの設定
        building->SetPosition(position);
        Vector3 scale = building->GetScale();
        scale.y = scaleDist(gen);
        building->SetScale(scale);

        buildings_.emplace_back(std::move(building));
    }
}
