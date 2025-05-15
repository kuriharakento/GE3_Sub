#include "ParticleEmitter.h"
#include "ParticleManager.h"
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
    Emit();

    for (auto& particle : particleGroup_->GetParticles())
    {
        for (auto& behavior : behaviorComponents_)
        {
            behavior->Update(particle);
        }
    }

    particleGroup_->Update(camera);
}

void ParticleEmitter::Draw(DirectXCommon* dxCommon, SrvManager* srvManager)
{
    if (!particleGroup_) return;
    particleGroup_->Draw(dxCommon, srvManager);
}

void ParticleEmitter::AddComponent(std::shared_ptr<IParticleBehaviorComponent> component)
{
    behaviorComponents_.push_back(component);
}

void ParticleEmitter::Start(const Vector3& position, uint32_t count, float duration, bool isLoop)
{
    isPlaying_ = true;
    position_ = position;
    emitCount_ = count;
    emitTime_ = 0.0f;
    timeSinceLastEmit_ = 0.0f;
    duration_ = duration;
    isLoop_ = isLoop;
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
            Particle newParticle;
            Vector3 randomOffset = MathUtils::RandomVector3(emitRangeMin_, emitRangeMax_);

            newParticle.transform.translate = position_ + randomOffset;
            newParticle.transform.scale = initialScale_;
            newParticle.transform.rotate = { 0.0f, 0.0f, 0.0f };
            newParticle.velocity = initialVelocity_;
            newParticle.color = initialColor_;
            newParticle.lifeTime = initialLifeTime_;
            newParticle.currentTime = 0.0f;

            particleGroup_->AddParticle(newParticle);
        }
        timeSinceLastEmit_ = 0.0f;
    }
}
