#pragma once
#include "Mech.h"
#include "application/Collision/ICollidable.h"

class Player : public Mech, public ICollidable
{
public:
	Player()
	{
		status_.health = 100.0f;
		status_.attackPower = 10.0f;
		status_.speed = 0.03f;
		hitBox_.min = Vector3(-0.5f, -0.5f, -0.5f);
		hitBox_.max = Vector3(0.5f, 0.5f, 0.5f);
		type_ = ObjectType::Player;
	}

	void Initialize(const std::string& filePath, Object3dCommon* objectCommon);

	void Update(CameraManager* camera = nullptr);

	void Draw();

	void OnCollision(ICollidable* other) override;

public: //アクセッサ

	ObjectType GetType() const override;

	float GetHealth() const override;

	float GetAttackPower() const override;

	float GetSpeed() const override;

	const AABB& GetBoundingBox() const override;

private:

	void UpdateObjTransform();

};

inline void Player::UpdateObjTransform()
{
	object3d_->SetScale(transform_.scale);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetTranslate(transform_.translate);
	object3d_->Update();
}

