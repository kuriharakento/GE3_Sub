#include "EnemyManager.h"

#include "externals/imgui/imgui.h"

void EnemyManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera, const std::string& filePath)
{
	//引数を受け取って初期化
	objectCommon_ = objectCommon;
	cameraManager_ = camera;
	filePath_ = filePath;
}

void EnemyManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("EnemyManager");
	if (ImGui::Button("Spawn Enemies"))
	{
		SpawnEnemies(5, Vector3(-5.0f, 0.0f, 7.0f), Vector3(3.0f, 0.0f, 0.0f));
	}
	if (ImGui::CollapsingHeader("Enemies"))
	{
		for (auto& enemy : enemies_)
		{
			//位置を変更
			Vector3 position = enemy->GetPosition();
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			enemy->SetPosition(position);
			//スケールを変更
			Vector3 scale = enemy->GetScale();
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			enemy->SetScale(scale);
			//回転を変更
			Vector3 rotate = enemy->GetRotate();
			ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
			enemy->SetRotate(rotate);
		}
	}
	ImGui::End();
#endif
	//敵の更新
	for (auto& enemy : enemies_)
	{
		enemy->Update(cameraManager_);
	}
}

void EnemyManager::Draw()
{
	//敵の描画
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

void EnemyManager::SpawnEnemies(int count, const Vector3& basePosition, const Vector3& offset)
{
	for (int i = 0; i < count; ++i)
	{
		// 新しい敵を生成してリストに追加
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(filePath_, objectCommon_);

		// 敵の位置を計算
		Vector3 position = basePosition + Vector3(offset.x * i, offset.y * i, offset.z * i);
		enemy->SetPosition(position);

		enemies_.push_back(std::move(enemy));
	}
}
