#pragma once
#include "GameObject.h"
class CameraManager;

class GameObjectManager
{
public:
	// シングルトン
	static GameObjectManager* GetInstance();
	// 初期化
	void Initialize();
	// 終了
	void Finalize();
	// 全てのGameObjectを更新
	void Update();
	// 全てのGameObjectを描画
	void Draw(CameraManager* camera);
	// GameObjectのポインタを登録する
	void Register(GameObject* gameObject);
	// GameObjectのポインタを削除する
	void Unregister(GameObject* gameObject);
	
private:
	// GameObjectのリスト
	std::vector<GameObject*> gameObjects_;
	// 削除予定のGameObjectリスト
	std::vector<GameObject*> pendingRemoval_;
};

