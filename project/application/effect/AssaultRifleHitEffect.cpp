#include "AssaultRifleHitEffect.h"

#include "effects/component/group/UVTranslateComponent.h"
#include "effects/component/single/ColorFadeOutComponent.h"
#include "effects/component/single/ScaleOverLifetimeComponent.h"

static uint32_t effectCount = 0; // エフェクトの識別子として使用

void AssaultRifleHitEffect::Initialize()
{
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("AssaultRifleHitEffect" + std::to_string(effectCount), "./Resources/gradationLine.png");
	effectCount++;
	emitter_->SetEmitRange({}, {});
	emitter_->SetInitialLifeTime(1.0f);
	emitter_->SetBillborad(true);
	//emitter_->SetRandomRotation(true);
	emitter_->SetRandomRotationRange(AABB{ Vector3{ -3.14f, 3.14f, 0.0f }, Vector3{ 3.14f, 3.14f, 0.0f } });
	emitter_->SetModelType(ParticleGroup::ParticleType::Ring); // 円環状のパーティクル

	//=======コンポーネントの追加=========================

	// スケール変化コンポーネント
	emitter_->AddComponent(std::make_shared<ScaleOverLifetimeComponent>(0.0f, 4.0f));
	// 色フェードアウトコンポーネント
	emitter_->AddComponent(std::make_shared<ColorFadeOutComponent>());
	// UVトランスレート
	emitter_->AddComponent(std::make_shared<UVTranslateComponent>(Vector3{ 0.1f, 0.0f, 0.0f })); // UVを毎フレーム大きくずらす
}

void AssaultRifleHitEffect::Play(const Vector3& position)
{
	if (!emitter_) return;
	// エミッターを開始
	emitter_->Start(position, 3, 0.0f, false); // 1回だけ放出
}
