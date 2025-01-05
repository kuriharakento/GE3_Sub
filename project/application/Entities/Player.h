#pragma once
#include "Mech.h"
#include "application/Collision/ICollidable.h"
#include "application/Weapon/MachineGun.h"

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

	void Initialize(const std::string& filePath, Object3dCommon* objectCommon,CameraManager* camera);

	void Update();

	void Draw();

	void OnCollision(ICollidable* other) override;

public: //アクセッサ

	ObjectType GetType() const override;

	float GetHealth() const override;

	float GetAttackPower() const override;

	float GetSpeed() const override;

	const AABB& GetBoundingBox() const override;

	Vector3 GetPosition() const override { return transform_.translate; }
	Vector3 GetScale() const { return transform_.scale; }
	Vector3 GetRotate() const { return transform_.rotate; }

	void SetPosition(const Vector3& position) override { transform_.translate = position; }
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

	//弾のリストを取得
	MachineGun* GetMachineGun() { return machineGun_.get(); }

private:

	void UpdateObjTransform();

	void CameraUpdate();

private: //メンバ変数
	CameraManager* cameraManager_ = nullptr;

	//プレイヤーの武器
	std::unique_ptr<MachineGun> machineGun_ = nullptr;

	float cameraZOffset_ = -4.0f;
};

inline void Player::UpdateObjTransform()
{
	object3d_->SetScale(transform_.scale);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetTranslate(transform_.translate);
	object3d_->Update(cameraManager_);
}

