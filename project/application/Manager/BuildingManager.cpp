#include "BuildingManager.h"

#include <random>
#include <unordered_set>

#include "3d/ModelManager.h"

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

	//建物を生成
	GenerateBuilding(10);
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

void BuildingManager::GenerateBuilding(int count)
{
	// 乱数生成器の初期化
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> posDist(-50.0f, 50.0f); // 位置の範囲
	std::uniform_real_distribution<float> scaleDist(0.5f, 10.0f);  // Y軸の大きさの範囲

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
		scale.x = scale.z = scaleDist(gen);
		scale.y = scaleDist(gen);
		building->SetScale(scale);
		building->UpdateAABB();

		buildings_.emplace_back(std::move(building));
	}
}
