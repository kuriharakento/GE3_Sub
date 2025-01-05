#pragma once
#include <vector>
#include "application/Collision/ICollidable.h"

class CollisionManager {
public:
	//当たり判定を行うオブジェクトの追加
    void AddCollidable(ICollidable* collidable);
	//初期化
	void Initialize();
	//更新
    void Update();
	//リストのクリア
	void Clear();

private:
	//当たり判定を行う
    bool CheckCollision(ICollidable* a, ICollidable* b);

	//プレイヤーと建物の当たり判定
	void HandlePlayerBuildingCollision(ICollidable* a, ICollidable* b);

	//当たり判定を行うオブジェクトのリスト
    std::vector<ICollidable*> collidables_;
};

