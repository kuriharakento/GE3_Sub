#include "Bullet.h"

Bullet::Bullet()
{
	status_.isAlive = true;
	status_.attackPower = 30.0f;
	status_.speed = 1.0f;
}

void Bullet::Initialize(Object3dCommon* object3dCommon, const Vector3& position, const Vector3& direction)
{
	//生成した弾を初期化
	bullet_ = std::make_unique<Object3d>();
	bullet_->Initialize(object3dCommon);

	//引数で受けとった座標を記録
	bullet_->SetTranslate(position);

	//モデルをセットする
	bullet_->SetModel(filePath_);

	//移動方向を記録
	velocity_ = direction;
	//向きを変更
	bullet_->SetRotate({ 0.0f, atan2f(velocity_.x, velocity_.z), 0.0f });

	//移動方向を正規化
	velocity_.Normalize();

	type_ = ObjectType::Bullet;

	//ヒットボックスの設定
	hitBox_.min = -bullet_->GetScale();
	hitBox_.max = bullet_->GetScale();
}

void Bullet::Update(CameraManager* camera)
{
	//生存時間のカウント
	elapsedTime_ += kDeltaTime;
	if (elapsedTime_ >= lifeTime_)
	{
		//寿命がすぎたのでフラグを下げる
		status_.isAlive = false;
	}

	Vector3 position = bullet_->GetTranslate();
	position += velocity_ * status_.speed;
	bullet_->SetTranslate(position);

	//行列の更新
	bullet_->Update(camera);
}

void Bullet::Draw()
{
	bullet_->Draw();
}

void Bullet::OnCollision(ICollidable* other)
{
	//プレイヤーに当たったら何もしない
	if (other->GetType() == ObjectType::Player ) { return; }

	//弾が当たったらフラグを下げる
	status_.isAlive = false;
}

float Bullet::GetAttackPower() const
{
	return status_.attackPower;
}
