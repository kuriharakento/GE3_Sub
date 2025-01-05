#pragma once
#include "3d/Object3d.h"
#include "application/Collision/ICollidable.h"
#include "application/Collision/Status.h"

class Bullet : public ICollidable
{
public:
	// 初期化
	void Initialize(Object3dCommon* object3dCommon, const Vector3& position, const Vector3& direction);

	// 更新
	void Update(CameraManager* camera);

	// 描画
	void Draw();

	// 弾が有効かどうか
	bool IsAlive() const { return status_.isAlive; }

	Vector3 GetPosition() const  override { return bullet_->GetTranslate(); }
	Vector3 GetRotation() const { return bullet_->GetRotate(); }
	Vector3 GetScale() const { return bullet_->GetScale(); }

	void SetPosition(const Vector3& position) override { bullet_->SetTranslate(position); }
	void SetRotation(const Vector3& rotation) { bullet_->SetRotate(rotation); }
	void SetScale(const Vector3& scale) { bullet_->SetScale(scale); }

	void OnCollision(ICollidable* other) override;

	ObjectType GetType() const override { return type_; }

	const AABB& GetBoundingBox() const override { return hitBox_; }

private:
	// 3Dオブジェクト
	std::unique_ptr<Object3d> bullet_;

	//移動方向ベクトル
	Vector3 velocity_ = {};

	//ステータス
	Status status_ = Status();

	//寿命
	float lifeTime_ = 3.0f;

	//経過時間
	float elapsedTime_ = 0.0f;

	const float kDeltaTime = 1.0f / 60.0f;

	//３Dモデルのファイルパス
	const std::string filePath_ = "plane.obj";

};

