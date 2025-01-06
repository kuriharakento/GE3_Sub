#pragma once

class SceneManager;

// IScene インターフェース
class IScene {
public:
	// シーンの状態
	enum class ScenePhase
	{
		Start,
		Play,
		End,
	};
	virtual ~IScene() = default;

	// 初期化
    virtual void Initialize(SceneManager* sceneManager) = 0;
	// 更新
    virtual void Update() = 0;
	// 描画
    virtual void Draw3D() = 0;
	virtual void Draw2D() = 0;

	// フェーズの切り替え
	void ChangePhase(ScenePhase newPhase) {
		currentPhase_ = newPhase;
		OnPhaseChanged(newPhase);
	}

protected:
	virtual void OnPhaseChanged(ScenePhase newPhase) = 0;

	// シーンマネージャーのポインタ
    SceneManager* sceneManager_;

	// シーンの状態
	ScenePhase currentPhase_;
};
