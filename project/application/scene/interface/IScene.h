#pragma once
class SceneManager;

class IScene
{
public: //メンバ関数
	//デストラクタ
	virtual ~IScene() = default;
	//初期化
	virtual void Initialize(SceneManager* sceneManager) = 0;
	//終了
	virtual void Finalize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw3D() = 0;
	virtual void Draw2D() = 0;
private:
	SceneManager* sceneManager_ = nullptr;
};