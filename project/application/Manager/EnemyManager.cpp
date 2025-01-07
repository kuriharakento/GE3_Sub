#include "EnemyManager.h"

#include <random>

#include "application/Entities/Player.h"
#include "base/Logger.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

void EnemyManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera, Player* player, const std::string& filePath)
{
	//引数を受け取って初期化
	objectCommon_ = objectCommon;
	cameraManager_ = camera;
	player_ = player;
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

	//死んだ敵を削除
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<Enemy>& enemy) {
				return !enemy->IsAlive();
			}),
		enemies_.end());
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
		enemy->SetPlayer(player_);

		enemies_.push_back(std::move(enemy));
	}
}

void EnemyManager::SpawnEnemies(int count, float minRadius, float maxRadius)
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
	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);

	for (int i = 0; i < count; ++i)
	{
		// ランダムな角度と半径を生成
		float angle = angleDist(gen);
		float radius = radiusDist(gen);

		// 円周上の位置を計算
		float x = radius * std::cos(angle);
		float z = radius * std::sin(angle);
		Vector3 position = { x, 1.0f, z };

		// 新しい敵を生成
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(filePath_, objectCommon_);

		// 位置を設定
		enemy->SetPosition(position);

		// 必要に応じてスケールや回転を設定
		enemy->SetScale(Vector3(1.0f, 1.0f, 1.0f));
		enemy->SetRotate(Vector3(0.0f, 0.0f, 0.0f));

		// プレイヤーを設定
		enemy->SetPlayer(player_);

		// 敵をリストに追加
		enemies_.emplace_back(std::move(enemy));
	}
}
