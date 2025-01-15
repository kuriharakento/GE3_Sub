#include "BuildingManager.h"

#include <random>
#include <unordered_set>

#include "3d/ModelManager.h"
#include "base/Logger.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

void BuildingManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera)
{
	//引数で受け取ったポインタを記録
	objectCommon_ = objectCommon;
	cameraManager_ = camera;

	//モデルの読み込み
	ModelManager::GetInstance()->LoadModel(filePath_);

	//リストの初期化
	buildings_.clear();
}

void BuildingManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("BuildingManager");
	if (ImGui::CollapsingHeader("Buildings")) {
		//建物の情報
		for (int i = 0; i < buildings_.size(); i++) {
			ImGui::Text("Building %d", i);
			//位置を変更
			Vector3 position = buildings_[i]->GetPosition();
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			buildings_[i]->SetPosition(position);
			//スケールを変更
			Vector3 scale = buildings_[i]->GetScale();
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			buildings_[i]->SetScale(scale);
			//回転を変更
			Vector3 rotate = buildings_[i]->GetRotate();
			ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
			buildings_[i]->SetRotate(rotate);
			//当たり判定
			AABB aabb = buildings_[i]->GetBoundingBox();
			ImGui::Text("Min %f %f %f", aabb.min.x, aabb.min.y, aabb.min.z);
			ImGui::Text("Max %f %f %f", aabb.max.x, aabb.max.y, aabb.max.z);
		}
	}
	ImGui::End();
#endif

	//全ての建物を更新
	for (auto& building : buildings_)
	{
		building->Update(cameraManager_);
	}

	//建物が破壊されたらリストから削除
	buildings_.erase(
		std::remove_if(buildings_.begin(), buildings_.end(),
			[](const std::unique_ptr<Building>& building)
			{
				return !building->IsAlive();
			}),
		buildings_.end());
}

void BuildingManager::Draw()
{
	//全ての建物を描画
	for (auto& building : buildings_)
	{
		building->Draw();
	}
}

void BuildingManager::GenerateBuilding(int count, float minRadius, float maxRadius)
{
    if (minRadius < 0.0f || maxRadius < 0.0f) {
        Logger::Log("最小半径および最大半径は0以上でなければなりません。");
        return;
    }

    if (minRadius > maxRadius && maxRadius != 0.0f) {
        Logger::Log("最小半径は最大半径以下でなければなりません。");
        return;
    }

    // 乱数生成器の初期化
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-50.0f, 50.0f); // 位置の範囲
	std::uniform_real_distribution<float> scaleXZDist(0.5f, 7.0f);  // スケールの範囲
    std::uniform_real_distribution<float> scaleYDist(1.0f, 12.0f);  // スケールの範囲

    // 配置済みの位置を記録するセット
    std::unordered_set<std::string> occupiedPositions;

    // カメラの位置を取得して中心点に設定
    Camera* activeCamera = cameraManager_->GetActiveCamera();
    if (!activeCamera) {
        Logger::Log("アクティブなカメラが存在しません。");
        return;
    }
    Vector3 cameraPosition = activeCamera->GetTranslate();
    Vector3 exclusionCenter = cameraPosition;

    // 建物を生成
    for (int i = 0; i < count; i++)
    {
        Vector3 position;
        std::string posKey;
        bool validPosition = false;
        int attempts = 0;
        const int maxAttempts = 100;

        // 有効な位置が見つかるまでループ
        while (!validPosition && attempts < maxAttempts)
        {
            float x = posDist(gen);
            float z = posDist(gen);
            position = { x, 0.0f, z };
            posKey = std::to_string(static_cast<int>(position.x)) + "_" + std::to_string(static_cast<int>(position.z));

            // 既に占有されている位置をスキップ
            if (occupiedPositions.find(posKey) != occupiedPositions.end())
            {
                attempts++;
                continue;
            }

            // カメラ位置からの距離を計算
            float distance = std::sqrtf(
                std::powf(position.x - exclusionCenter.x, 2) +
                std::powf(position.y - exclusionCenter.y, 2) +
                std::powf(position.z - exclusionCenter.z, 2)
            );

            bool withinMinRadius = distance >= minRadius;
            bool withinMaxRadius = (maxRadius > 0.0f) ? (distance <= maxRadius) : true;

            if (withinMinRadius && withinMaxRadius)
            {
                validPosition = true;
            } else
            {
                attempts++;
            }
        }

        if (validPosition)
        {
            // 位置を記録
            occupiedPositions.insert(posKey);

            // 建物の生成と初期化
            auto building = std::make_unique<Building>();
            building->Initialize(filePath_, objectCommon_);

            // 位置とスケールの設定
            building->SetPosition(position);
            Vector3 scale = building->GetScale();
            scale.x = scale.z = scaleXZDist(gen);
            scale.y = scaleYDist(gen);
            building->SetScale(scale);
            building->UpdateAABB();

            buildings_.emplace_back(std::move(building));
        } else
        {
            Logger::Log("有効なビルの位置が見つかりませんでした。");
        }
    }
}