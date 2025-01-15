#pragma once
class SceneManager;

class BaseScene
{
public: //メンバ関数
	// シーンの状態
	enum class ScenePhase
	{
		Start,
		Play,
		End,
	};
	//コンストラクタ
	BaseScene() = default;
	//デストラクタ
	virtual ~BaseScene() = default;
	//初期化
	virtual void Initialize() = 0;
	//終了
	virtual void Finalize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw3D() = 0;
	virtual void Draw2D() = 0;

	// フェーズの切り替え
	void ChangePhase(ScenePhase newPhase) {
		currentPhase_ = newPhase;
		OnPhaseChanged(newPhase);
	}

	//シーンマネージャーのセット
	void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

protected:
	//フェーズの切り替えたときの処理
	virtual void OnPhaseChanged(ScenePhase newPhase) = 0;

	// シーンマネージャーのポインタ
	SceneManager* sceneManager_ = nullptr;

	// シーンの状態
	ScenePhase currentPhase_;
};