# GravityComponent 使用例

## 基本的な使用方法

`GravityComponent` は物理的な重力を GameObjects に適用するコンポーネントです。

### 1. 基本的な重力の適用

```cpp
#include "application/GameObject/component/physics/GravityComponent.h"
#include "application/GameObject/base/GameObject.h"

// GameObjectを初期化
GameObject* object = new GameObject("GravityObject");
object->SetPosition({0.0f, 10.0f, 0.0f});

// デフォルトの重力（9.8f）を適用
object->AddComponent("GravityComponent", std::make_unique<GravityComponent>());
```

### 2. カスタム重力の適用

```cpp
// より強い重力（19.6f）を適用
object->AddComponent("GravityComponent", std::make_unique<GravityComponent>(19.6f));
```

### 3. プレイヤーに重力を適用

```cpp
// Player.cpp の Initialize() メソッド内
void Player::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
    Character::Initialize(object3dCommon, lightManager);
    
    // 移動コンポーネントを追加
    AddComponent("MoveComponent", std::make_unique<MoveComponent>());
    
    // 重力コンポーネントを追加
    AddComponent("GravityComponent", std::make_unique<GravityComponent>());
    
    // その他のコンポーネント...
}
```

### 4. 敵キャラクターに重力を適用

```cpp
// Enemy.cpp の Initialize() メソッド内
void Enemy::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
    Character::Initialize(object3dCommon, lightManager);
    
    // 重力コンポーネントを追加（月の重力程度）
    AddComponent("GravityComponent", std::make_unique<GravityComponent>(1.6f));
    
    // その他のコンポーネント...
}
```

## 重要事項

- `GravityComponent` は `IActionComponent` を継承しており、各フレームで自動的に更新されます
- 重力は Y 軸方向（垂直方向）にのみ適用されます
- フレームレートは 60fps（deltaTime = 1.0f/60.0f）として計算されます
- 重力は加速度として適用され、時間とともに落下速度が増加します

## 単一責任原則の遵守

`GravityComponent` は重力処理のみを担当し、移動や衝突判定などの他の責務は持ちません。
これにより、以下の利点があります：

- コードの保守性向上
- 機能の組み合わせが容易
- テストが簡単
- 将来的な拡張が容易