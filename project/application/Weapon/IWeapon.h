#pragma once
#include "math/VectorFunc.h"

class IWeapon
{
public:
    virtual ~IWeapon() = default;

    // 武器を発射する
    virtual void Shoot() = 0;

    // 現在の弾薬数を取得
    virtual int GetCurrentAmmo() const = 0;

    // リロード中かどうか
    virtual bool IsReloading() const = 0;

    // リロードを開始
    virtual void StartReload() = 0;

    // リロード完了処理
    virtual void FinishReload() = 0;
};
