#pragma once

#include "3d/Object3d.h"

//ステータス情報をまとめた構造体
//NOTE:ここは必要に応じて増やしていく
struct Status
{
	float health;			//体力
	float attackPower;		//攻撃力
	float speed;			//移動速度

	//コンストラクタで初期化
	Status() : health(0.0f), attackPower(0.0f) {}
};

//機体クラス。プレイヤーと敵の基底クラス
class Mech
{
protected:
	//ステータス情報
	Status status_;
	//モデル
	std::unique_ptr<Object3d> object3d_;
	//座標
	Transform transform_;
};

