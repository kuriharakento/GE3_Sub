#pragma once

#include "3d/Object3d.h"
#include "application/Collision/Status.h"

//機体クラス。プレイヤーと敵の基底クラス
class Mech
{
protected:
	//ステータス情報
	Status status_;
	//モデル
	std::unique_ptr<Object3d> object3d_;
	//座標
	Transform transform_ = {
		{ 1.0f,1.0f,1.0f },
		{},
		{},
	};

};

