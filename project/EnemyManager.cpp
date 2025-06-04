#include "EnemyManager.h"

#include "application/GameObject/character/enemy/AssaultEnemy.h"
#include "application/GameObject/character/enemy/PistolEnemy.h"
#include "application/GameObject/character/enemy/ShotgunEnemy.h"
#include "math/MathUtils.h"

void EnemyManager::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target)
{
	object3dCommon_ = object3dCommon; // 3Dオブジェクト共通処理
	lightManager_ = lightManager; // ライトマネージャー
	target_ = target; // ターゲット（プレイヤーなど）
	enemies_.clear(); // 敵キャラクターのリストをクリア
	// 敵キャラクターの出現範囲を設定
	emitRange_ = {
		{ -10.0f, 1.0f, -10.0f }, // 最小座標
		{ 10.0f, 1.0f, 10.0f }   // 最大座標
	};
}

void EnemyManager::Update()
{
	for (auto& enemy : enemies_)
	{
		enemy->Update(); // 各敵キャラクターの更新
	}
}

void EnemyManager::Draw(CameraManager* camera)
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw(camera); // 各敵キャラクターの描画
	}
}

void EnemyManager::AddPistolEnemy(uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		auto enemy = std::make_unique<PistolEnemy>();
		enemy->Initialize(object3dCommon_, lightManager_, target_);
		//ランダムな位置を設定
		Vector3 randomPosition = MathUtils::RandomVector3(emitRange_.min_, emitRange_.max_);
		enemy->SetPosition(randomPosition);
		// 敵キャラクターを追加
		enemies_.push_back(std::move(enemy));
	}
}

void EnemyManager::AddAssaultEnemy(uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		auto enemy = std::make_unique<AssaultEnemy>();
		enemy->Initialize(object3dCommon_, lightManager_, target_);
		//ランダムな位置を設定
		Vector3 randomPosition = MathUtils::RandomVector3(emitRange_.min_, emitRange_.max_);
		enemy->SetPosition(randomPosition);
		// 敵キャラクターを追加
		enemies_.push_back(std::move(enemy));
	}
}

void EnemyManager::AddShotgunEnemy(uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		auto enemy = std::make_unique<ShotgunEnemy>();
		enemy->Initialize(object3dCommon_, lightManager_, target_);
		//ランダムな位置を設定
		Vector3 randomPosition = MathUtils::RandomVector3(emitRange_.min_, emitRange_.max_);
		enemy->SetPosition(randomPosition);
		// 敵キャラクターを追加
		enemies_.push_back(std::move(enemy));
	}
}
