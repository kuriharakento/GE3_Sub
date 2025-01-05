#pragma once
#include "3d/Object3d.h"
#include "application/Collision/ICollidable.h"
#include "base/GraphicsTypes.h"
#include "math/VectorFunc.h"

class CameraManager;
class Building : public ICollidable {
public:
    // コンストラクタ
    Building();

	// 初期化処理
	void Initialize(const std::string& filePath, Object3dCommon* objectCommon);

    void Update(CameraManager* camera);

	void Draw();

    // AABBの座標情報を取得
    const AABB& GetBoundingBox() const override;

    // 位置の取得
    Vector3 GetPosition() const override;
	Vector3 GetScale() const { return transform_.scale; }
	Vector3 GetRotate() const { return transform_.rotate; }

    void SetPosition(const Vector3& position) override;
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

    // 衝突時のコールバック関数
    void OnCollision(ICollidable* other) override;

    // オブジェクトの種類を取得
    ObjectType GetType() const override;

private:
	void UpdateOBJTransform(CameraManager* camera);

private:
    //トランスフォーム
	Transform transform_;

    //３Dモデル
	std::unique_ptr<Object3d> object3d_;

};
