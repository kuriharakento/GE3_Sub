#pragma once

#include "math/Vector3.h"
#include <memory>

#include "application/GameObject/base/GameObject.h"

class Bullet : public GameObject {
public:
	Bullet(std::string tag) : GameObject(tag), isAlive_(true) {}
    virtual ~Bullet();

    void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, const Vector3& position);
    void Update(float deltaTime);
    void Draw(CameraManager* camera);

	void SetActive(bool active) { isAlive_ = active; }
    bool IsAlive() const { return isAlive_; }

private:
    bool isAlive_;       // 生死状態
};
