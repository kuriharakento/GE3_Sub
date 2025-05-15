#pragma once
#include <memory>
#include "ParticleGroup.h"

class ParticleEmitter
{
public:
	~ParticleEmitter();
	void Initialize(const std::string& groupName, const std::string& textureFilePath);
	void Update(CameraManager* camera);
	void Draw(DirectXCommon* dxCommon, SrvManager* srvManager);
	void AddComponent(std::shared_ptr<IParticleBehaviorComponent> component);

	void Start(const Vector3& position, uint32_t count, float duration, bool isLoop = false);
	void StopEmit();
	void SetEmitRange(const Vector3& min, const Vector3& max);
	void SetEmitRate(float rate) { emitRate_ = rate; }
	void SetEmitCount(uint32_t count) { emitCount_ = count; }
	void SetLoop(bool loop) { isLoop_ = loop; }

private:
	void Emit();

protected:
	// パーティクルのグループ名
	std::string groupName_ = "";
	// パーティクルグループ
	std::unique_ptr<ParticleGroup> particleGroup_ = nullptr;
	//　コンポーネントのリスト
	std::list<std::shared_ptr<IParticleBehaviorComponent>> behaviorComponents_;
	// パーティクルの発生位置
	Vector3 position_ = {};
	// 発生範囲の最小,最大座標
	Vector3 emitRangeMin_ = {};
	Vector3 emitRangeMax_ = {};
	// 発生頻度
	float emitRate_ = 5.0f;
	// 最後生成してからの経過時間
	float timeSinceLastEmit_ = 0.0f;
	//発生させる数
	uint32_t emitCount_ = 3;
	// ループフラグ
	bool isLoop_ = false;
	// 発生させてるか
	bool isPlaying_ = false;
	// 発生させる時間
	float emitTime_ = 0.0f;
	// 継続時間
	float duration_ = 0.0f;
};

