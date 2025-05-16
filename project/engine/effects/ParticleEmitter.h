#pragma once
#include <memory>
#include <list>
#include <string>
#include "ParticleGroup.h"
#include "effects/component/interface/IParticleComponent.h"

class ParticleEmitter
{
public:
    ~ParticleEmitter();
    void Initialize(const std::string& groupName, const std::string& textureFilePath);
    void Update(CameraManager* camera);
    void Draw(DirectXCommon* dxCommon, SrvManager* srvManager);
    void AddComponent(std::shared_ptr<IParticleComponent> component);

    void Start(const Vector3& position, uint32_t count, float duration, bool isLoop = false);
    void StopEmit();
    void SetEmitRange(const Vector3& min, const Vector3& max);
    void SetEmitRate(float rate) { emitRate_ = rate; }
    void SetEmitCount(uint32_t count) { emitCount_ = count; }
    void SetLoop(bool loop) { isLoop_ = loop; }
	void SetBillborad(bool flag) { particleGroup_->SetBillboard(flag); }
	void SetTexture(const std::string& textureFilePath) { particleGroup_->SetTexture(textureFilePath); }
	void SetModelType(ParticleGroup::ParticleType type) { particleGroup_->SetModelType(type); }
	Vector3 GetUVTranslate() const { return particleGroup_->GetUVTranslate(); }
	void SetUVTranslate(const Vector3& translate) { particleGroup_->SetUVTranslate(translate); }
	Vector3 GetUVScale() const { return particleGroup_->GetUVScale(); }
	void SetUVScale(const Vector3& scale) { particleGroup_->SetUVScale(scale); }
	Vector3 GetUVRotate() const { return particleGroup_->GetUVRotate(); }
	void SetUVRotate(const Vector3& rotate) { particleGroup_->SetUVRotate(rotate); }

    // 初期パラメータのアクセッサ
    void SetInitialLifeTime(float time) { initialLifeTime_ = time; }
    void SetInitialVelocity(const Vector3& velocity) { initialVelocity_ = velocity; }
    void SetInitialColor(const Vector4& color) { initialColor_ = color; }
    void SetInitialScale(const Vector3& scale) { initialScale_ = scale; }

    float GetInitialLifeTime() const { return initialLifeTime_; }
    Vector3 GetInitialVelocity() const { return initialVelocity_; }
    Vector4 GetInitialColor() const { return initialColor_; }
    Vector3 GetInitialScale() const { return initialScale_; }

private:
    void Emit();

private:
    std::string groupName_ = "";
    std::unique_ptr<ParticleGroup> particleGroup_ = nullptr;
    std::list<std::shared_ptr<IParticleComponent>> behaviorComponents_;

    Vector3 position_ = {};
    Vector3 emitRangeMin_ = {};
    Vector3 emitRangeMax_ = {};

    float emitRate_ = 2.0f;
    float timeSinceLastEmit_ = 0.0f;
    uint32_t emitCount_ = 3;
    bool isLoop_ = false;
    bool isPlaying_ = false;
    float emitTime_ = 0.0f;
    float duration_ = 0.0f;

    // --- 初期化用プロパティ ---
    float initialLifeTime_ = 2.0f;
    Vector3 initialVelocity_ = { 0.0f, 0.0f, 0.0f };
    Vector4 initialColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector3 initialScale_ = { 1.0f, 1.0f, 1.0f };
};
