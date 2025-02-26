#include "EnemyManager.h"

#include "imgui/imgui.h"

void EnemyManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera, LightManager* lightManager, const std::string& tackleEnemy)
{
	objectCommon_ = objectCommon;
	cameraManager_ = camera;
	lightManager_ = lightManager;
	tackleEnemyPath_ = tackleEnemy;
}

void EnemyManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("EnemyManager");
	ImGui::Text("TackleEnemyCount: %d", tackleEnemies_.size());
	ImGui::DragFloat3("TargetPosition", &targetPosition_.x, 0.1f);
	if (ImGui::Button("SpawnTackleEnemy"))
	{
		SpawnTackleEnemy(1);
	}
	if (ImGui::Button("Tackle"))
	{
		for (auto& enemy : tackleEnemies_)
		{
			enemy->StartTackle(targetPosition_);
		}
	}
	ImGui::End();
#endif
	//タックルエネミーの更新
	for (auto& enemy : tackleEnemies_)
	{
		enemy->Update(cameraManager_);
	}
}

void EnemyManager::Draw()
{
	//タックルエネミーの描画
	for (auto& enemy : tackleEnemies_)
	{
		enemy->Draw();
	}
}

void EnemyManager::SpawnTackleEnemy(uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		auto enemy = std::make_unique<TackleEnemy>();
		enemy->Initialize(objectCommon_, tackleEnemyPath_);
		enemy->SetLightManager(lightManager_);
		tackleEnemies_.emplace_back(std::move(enemy));
	}
}
