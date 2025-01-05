#pragma once
#include "3d/Object3d.h"
#include "math/VectorFunc.h"
#include "application/Collision/ICollidable.h"

class Missile : public ICollidable
{
public:
    // 初期化
    void Initialize(Object3dCommon* object3dCommon, const Vector3& startPosition, const Vector3& controlPoint, const Vector3& endPosition);

    // 更新
    void Update(CameraManager* camera);

    // 描画
    void Draw();

    // ミサイルが有効かどうか
    bool IsAlive() const { return isAlive_; }

    void OnCollision(ICollidable* other) override;

    ObjectType GetType() const override { return type_; }
    const AABB& GetBoundingBox() const override { return hitBox_; }

	Vector3 GetPosition() const override { return missile_->GetTranslate(); }
	Vector3 GetRotation() const { return missile_->GetRotate(); }
	Vector3 GetScale() const { return missile_->GetScale(); }

	void SetPosition(const Vector3& position) override { missile_->SetTranslate(position); }
	void SetRotation(const Vector3& rotation) { missile_->SetRotate(rotation); }
	void SetScale(const Vector3& scale) { missile_->SetScale(scale); }

private:
    // 3Dオブジェクト
    std::unique_ptr<Object3d> missile_;

    // ベジェ曲線の制御点
    Vector3 startPoint_;
    Vector3 controlPoint_;
    Vector3 endPoint_;

    // 速度
    float speed_ = 2.0f;

    // ターゲット位置
    Vector3 targetPosition_ = {};

    // ミサイルが有効かどうか
    bool isAlive_ = true;

    // 寿命
    float lifeTime_ = 5.0f;

    // 経過時間
    float elapsedTime_ = 0.0f;

    // デルタタイム
    const float kDeltaTime = 1.0f / 60.0f;

    // 3Dモデルのファイルパス
    const std::string filePath_ = "plane.obj";
};
