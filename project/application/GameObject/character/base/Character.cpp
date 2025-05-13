#include "Character.h"

void Character::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
    GameObject::Initialize(object3dCommon, lightManager);
}

void Character::Update()
{
    GameObject::Update();

    // 無敵時間の更新
    if (isInvincible_)
    {
        invincibleTimer_ -= 1.0f / 60.0f; // フレームレートに応じて減少
        if (invincibleTimer_ <= 0.0f)
        {
            invincibleTimer_ = 0.0f;
            isInvincible_ = false; // 無敵状態解除
        }
    }
}

void Character::Draw(CameraManager* camera)
{
    GameObject::Draw(camera);
}

void Character::TakeDamage(int damage)
{
    if (isInvincible_) return; // 無敵状態ならダメージを受けない

    hp_ -= damage;
    if (hp_ <= 0)
    {
        hp_ = 0;
        isAlive_ = false; // 死亡フラグを設定
    }
}

void Character::SetInvincible(float duration)
{
    isInvincible_ = true;
    invincibleTimer_ = duration; // 無敵時間を設定
}

bool Character::IsAlive() const
{
    return isAlive_;
}
