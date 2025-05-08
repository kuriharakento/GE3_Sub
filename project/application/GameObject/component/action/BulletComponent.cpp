#include "BulletComponent.h"  
#include "application/GameObject/base/GameObject.h" // GameObject の完全な型を使用するために追加  

BulletComponent::BulletComponent()  
   : speed_(0.0f), lifetime_(0.0f), timeAlive_(0.0f) {  
}  

void BulletComponent::Initialize(Vector3 direction, float speed, float lifetime) {  
   direction_ = direction;  
   speed_ = speed;  
   lifetime_ = lifetime;  
   timeAlive_ = 0.0f;  
}  

void BulletComponent::Update(GameObject* owner) {  
   timeAlive_ += 1.0f / 60.0f;  // 経過時間を更新  

   // 弾の移動  
   owner->SetPosition(owner->GetPosition() + direction_ * speed_ * (1.0f / 60.0f));  

   // ライフタイムを超えたら弾を削除  
   if (timeAlive_ >= lifetime_) {  
       owner->SetActive(false);  // 弾を非アクティブにすることで削除処理を行う  
   }  
}