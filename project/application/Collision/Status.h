#pragma once

//ステータス情報をまとめた構造体
//NOTE:ここは必要に応じて増やしていく
struct Status
{
	bool isAlive;	//生存フラグ
	float health;			//体力
	float attackPower;		//攻撃力
	float speed;			//移動速度

	//コンストラクタで初期化
	Status() : isAlive(true), health(0.0f), attackPower(0.0f), speed(0.0f) {}
};