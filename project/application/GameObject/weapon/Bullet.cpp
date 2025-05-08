#include "Bullet.h"

Bullet::Bullet() : direction_(), speed_(0.0f), lifetime_(0.0f), elapsedTime_(0.0f), isAlive_(true)
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, const Vector3& position, const Vector3& direction, float speed, float lifetime) {
	GameObject::Initialize(object3dCommon,lightManager); // GameObjectの初期化
    SetPosition(position);
    direction_ = direction.Normalize();
    speed_ = speed;
    lifetime_ = lifetime;
    elapsedTime_ = 0.0f;
    isAlive_ = true;
}

void Bullet::Update(float deltaTime) {
    if (!isAlive_) return;

    // 弾の位置を移動
    Vector3 newPosition = GetPosition() + direction_ * speed_ * deltaTime;
    SetPosition(newPosition);

    // 経過時間を更新
    elapsedTime_ += deltaTime;

    // 寿命が尽きたら弾を消す
    if (elapsedTime_ >= lifetime_) {
        isAlive_ = false;
    }
}

void Bullet::Draw(CameraManager* camera) {
    // 弾の描画処理（GameObjectの描画機能を使用）
    GameObject::Draw(camera);
}
