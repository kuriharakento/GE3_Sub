#pragma once
#include <vector>
#include "application/Collision/ICollidable.h"

class CollisionManager {
public:
    void AddCollidable(ICollidable* collidable);
	void Initialize();
    void Update();
	void Clear();

private:
    bool CheckCollision(ICollidable* a, ICollidable* b);

	//プレイヤーと建物の当たり判定
	void HandlePlayerBuildingCollision(ICollidable* a, ICollidable* b);

    std::vector<ICollidable*> collidables_;
};

