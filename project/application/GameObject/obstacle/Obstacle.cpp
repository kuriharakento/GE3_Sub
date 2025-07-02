#include "Obstacle.h"

#include "application/GameObject/component/collision/OBBColliderComponent.h"
#include "base/Logger.h"
#include "math/OBB.h"

void Obstacle::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	// ゲームオブジェクトの初期化
	GameObject::Initialize(object3dCommon, lightManager);
}

void Obstacle::Update()
{
	// ゲームオブジェクトの更新
	GameObject::Update();
}

void Obstacle::Draw(CameraManager* camera)
{
	// ゲームオブジェクトの描画処理
	GameObject::Draw(camera);
}

void Obstacle::AddComponent(const std::string& name, std::unique_ptr<IGameObjectComponent> comp)
{
	if (auto collider = dynamic_cast<ICollisionComponent*>(comp.get()))
	{
		// 衝突判定コンポーネントの場合は、衝突時の処理を設定
		CollisionSettings(collider);
	}
	// コンポーネントを追加
	GameObject::AddComponent(name, std::move(comp));
}

void Obstacle::CollisionSettings(ICollisionComponent* collider)
{
	// 衝突開始時
	collider->SetOnEnter([this](GameObject* other) {
		if (other->GetTag() == "Player" || other->GetTag() == "Enemy")
		{
			ResolvePenetration(other);
		}
						 });

	// 衝突継続中
	collider->SetOnStay([this](GameObject* other) {
		// プレイヤーか敵の場合のみ処理
		if (other->GetTag() == "Player" || other->GetTag() == "Enemy")
		{
			ResolvePenetration(other);
		}
						});

	// 衝突終了時
	collider->SetOnExit([this](GameObject* other) {

						});
}

// 修正しためり込み解決処理
void Obstacle::ResolvePenetration(GameObject* other)
{
	// タグチェック
	if (other->GetTag() != "Player" && other->GetTag() != "Enemy")
		return;

	// コライダーの取得（GetComponentの使用方法を修正）
	auto obstacleCollider = GetComponent<OBBColliderComponent>();
	auto otherCollider = other->GetComponent<OBBColliderComponent>();

	if (!obstacleCollider || !otherCollider)
	{
		return;
	}

	// OBBの取得
	const OBB& obstacleOBB = obstacleCollider->GetOBB();
	const OBB& otherOBB = otherCollider->GetOBB();

	// 両方のOBBの中心位置を取得
	Vector3 obstacleCenter = obstacleOBB.center; // 障害物の中心位置
	Vector3 otherCenter = otherOBB.center; // 相手の中心位置

	// 障害物から相手への方向ベクトル
	Vector3 direction = otherCenter - obstacleCenter;
	float distance = direction.Length();

	// 方向ベクトルを正規化
	Vector3 normalizedDir;
	if (distance > 0.001f) // ゼロ除算防止
	{
		normalizedDir = direction * (1.0f / distance);
	}
	else
	{
		normalizedDir = Vector3(0, 1, 0); // デフォルト方向
	}

	// 両方のOBBのサイズを取得
	Vector3 obstacleExtent = obstacleOBB.size;
	Vector3 otherExtent = otherOBB.size;

	// OBBの合計サイズを方向に投影
	float totalExtent =
		std::abs(obstacleExtent.x * normalizedDir.x) +
		std::abs(obstacleExtent.y * normalizedDir.y) +
		std::abs(obstacleExtent.z * normalizedDir.z) +
		std::abs(otherExtent.x * normalizedDir.x) +
		std::abs(otherExtent.y * normalizedDir.y) +
		std::abs(otherExtent.z * normalizedDir.z);

	// めり込んでいるかチェック
	if (distance < totalExtent)
	{
		// めり込み量を計算
		float penetration = totalExtent - distance;

		// 相手を押し出す
		Vector3 pushVector = normalizedDir * (penetration); // 少し余分に押し出す
		other->SetPosition(otherCenter + pushVector);
	}
}