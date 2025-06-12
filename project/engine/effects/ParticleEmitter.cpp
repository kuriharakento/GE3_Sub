#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "component/interface/IParticleGroupComponent.h"
#include "lighting/VectorColorCodes.h"
#include "line/LineManager.h"
#include "math/MathUtils.h"

ParticleEmitter::~ParticleEmitter()
{
	particleGroup_.reset();
	behaviorComponents_.clear();
	ParticleManager::GetInstance()->UnregisterEmitter(groupName_);
}

void ParticleEmitter::Initialize(const std::string& groupName, const std::string& textureFilePath)
{
	groupName_ = groupName;
	particleGroup_ = std::make_unique<ParticleGroup>();
	particleGroup_->Initialize(groupName, textureFilePath);
	ParticleManager::GetInstance()->RegisterEmitter(groupName_, this);
}

void ParticleEmitter::Update(CameraManager* camera)
{
	// 発生位置の更新
	UpdateEmitPosition();

	// パーティクル生成
	Emit();

	// パーティクル単体に作用するコンポーネントの更新
	for (auto& particle : particleGroup_->GetParticles())
	{
		for (auto& behavior : behaviorComponents_)
		{
			if (auto behaviorComponent = std::dynamic_pointer_cast<IParticleBehaviorComponent>(behavior))
			{
				behaviorComponent->Update(particle);
			}
		}
	}

	// パーティクルグループ全体に作用するコンポーネントの更新
	for (auto& behavior : behaviorComponents_)
	{
		if (auto groupComponent = std::dynamic_pointer_cast<IParticleGroupComponent>(behavior))
		{
			groupComponent->Update(*particleGroup_);
		}
	}

	particleGroup_->Update(camera);
}

void ParticleEmitter::Draw(DirectXCommon* dxCommon, SrvManager* srvManager)
{
#ifdef _DEBUG
	// 発生ポイントを描画
	LineManager::GetInstance()->DrawSphere(
		position_,
		0.1f,
		VectorColorCodes::Red
	);
	LineManager::GetInstance()->DrawAABB(
		AABB(
			position_ + emitRangeMin_,
			position_ + emitRangeMax_),
		VectorColorCodes::Green
	);
#endif

	if (!particleGroup_) return;
	particleGroup_->Draw(dxCommon, srvManager);
}

void ParticleEmitter::AddComponent(std::shared_ptr<IParticleComponent> component)
{
	behaviorComponents_.push_back(component);
}

void ParticleEmitter::Play()
{
	isPlaying_ = true;
	if (target_)
	{
		position_ = *target_;
	}
	emitTime_ = 0.0f;
	timeSinceLastEmit_ = emitRate_;
	// 初回の発生を即座に行う
	EmitFirst();
}

void ParticleEmitter::Start(const Vector3& position, uint32_t count, float duration, bool isLoop)
{
	isPlaying_ = true;
	target_ = nullptr;
	position_ = position;
	emitCount_ = count;
	emitTime_ = 0.0f;
	timeSinceLastEmit_ = emitRate_;
	duration_ = duration;
	isLoop_ = isLoop;
	//初回の発生を即座に行う
	EmitFirst();
}

void ParticleEmitter::Start(const Vector3* target, uint32_t count, float duration, bool isLoop)
{
	target_ = target;
	if (target)
	{
		position_ = *target_;
	}
	isPlaying_ = true;
	emitCount_ = count;
	emitTime_ = 0.0f;
	timeSinceLastEmit_ = emitRate_;
	duration_ = duration;
	isLoop_ = isLoop;
	// 初回の発生を即座に行う
	EmitFirst();
}

void ParticleEmitter::StopEmit()
{
	isPlaying_ = false;
	emitTime_ = 0.0f;
	timeSinceLastEmit_ = 0.0f;
}

void ParticleEmitter::SetEmitRange(const Vector3& min, const Vector3& max)
{
	emitRangeMin_ = min;
	emitRangeMax_ = max;
}

void ParticleEmitter::Emit()
{
	if (!isPlaying_) return;

	emitTime_ += 1.0f / 60.0f;
	timeSinceLastEmit_ += 1.0f / 60.0f;

	if (emitTime_ >= duration_)
	{
		if (isLoop_)
		{
			emitTime_ = 0.0f;
		}
		else
		{
			isPlaying_ = false;
			return;
		}
	}

	if (timeSinceLastEmit_ >= emitRate_)
	{
		for (uint32_t i = 0; i < emitCount_; ++i)
		{
			RandomizeInitialParameters();
			Particle newParticle;
			Vector3 randomOffset = MathUtils::RandomVector3(emitRangeMin_, emitRangeMax_);

			newParticle.transform.translate = position_ + randomOffset;
			newParticle.transform.scale = initialScale_;
			newParticle.transform.rotate = initialRotation_;
			newParticle.velocity = initialVelocity_;
			newParticle.color = initialColor_;
			newParticle.lifeTime = initialLifeTime_;
			newParticle.currentTime = 0.0f;

			particleGroup_->AddParticle(newParticle);
		}
		timeSinceLastEmit_ = 0.0f;
	}
}

void ParticleEmitter::EmitFirst()
{
	if (!isPlaying_) return;
	// 初回の発生を即座に行う
	for (uint32_t i = 0; i < emitCount_; ++i)
	{
		RandomizeInitialParameters();
		Particle newParticle;
		Vector3 randomOffset = MathUtils::RandomVector3(emitRangeMin_, emitRangeMax_);
		newParticle.transform.translate = position_ + randomOffset;
		newParticle.transform.scale = initialScale_;
		newParticle.transform.rotate = initialRotation_;
		newParticle.velocity = initialVelocity_;
		newParticle.color = initialColor_;
		newParticle.lifeTime = initialLifeTime_;
		newParticle.currentTime = 0.0f;
		particleGroup_->AddParticle(newParticle);
	}
}

void ParticleEmitter::UpdateEmitPosition()
{
	// 追従対象が設定されている場合、エミッターの位置を更新
	if (target_)
	{
		position_ = *target_;
	}
}

void ParticleEmitter::RandomizeInitialParameters()
{
	if (isRandomVelocity_)
	{
		initialVelocity_ = MathUtils::RandomVector3(randomVelocityRange_.min_, randomVelocityRange_.max_);
	}
	if (isRandomScale_)
	{
		initialScale_ = MathUtils::RandomVector3(randomScaleRange_.min_, randomScaleRange_.max_);
	}
	if (isRandomColor_)
	{
		initialColor_ = MathUtils::RandomVector4(randomColormin_, randomColormax_);
	}
	if (isRandomRotation_)
	{
		initialRotation_ = MathUtils::RandomVector3(randomRotationRange_.min_, randomRotationRange_.max_);
	}
}
