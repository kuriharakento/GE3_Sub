#pragma once

#include <application/GameObject/base/GameObject.h>
#include "math/Vector3.h"

class Character
{
public:
    virtual ~Character() = default;
	Character(std::string tag) : gameObject_(std::make_unique<GameObject>(tag)) {}
    virtual void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager);
    virtual void Update();
    virtual void Draw(CameraManager* camera);

    // HP
    void TakeDamage(int damage);
    bool IsAlive() const;

    // トランスフォーム
    const Vector3& GetPosition() const { return gameObject_->GetPosition(); }
    void SetPosition(const Vector3& position) { gameObject_->SetPosition(position); }
    const Vector3& GetRotation() const { return gameObject_->GetRotation(); }
    void SetRotation(const Vector3& rotation) { gameObject_->SetRotation(rotation); }
    const Vector3& GetScale() const { return gameObject_->GetScale(); }
    void SetScale(const Vector3& scale) { gameObject_->SetScale(scale); }

    // ステータスの設定
    void SetHp(float hp) { hp_ = hp; }
    float GetHp() const { return hp_; }

    // 無敵状態
    void SetInvincible(float duration); // 無敵状態を設定
    bool IsInvincible() const { return isInvincible_; }

    // 操作可能状態
    void SetControllable(bool controllable) { isControllable_ = controllable; }
    bool IsControllable() const { return isControllable_; }

protected:
    std::unique_ptr<GameObject> gameObject_;    // キャラクターの基盤となるGameObject

    // 基本ステータス
    float hp_ = 100.0f;
    float maxHp_ = 100.0f;
    float speed_ = 5.0f;

    // 状態管理
    bool isAlive_ = true;
    bool isInvincible_ = false;
    float invincibleTimer_ = 0.0f; // 無敵時間の残り
    bool isControllable_ = true;   // 操作可能フラグ
};
