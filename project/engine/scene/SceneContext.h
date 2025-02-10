#pragma once

class LineManager;
class LightManager;
class CameraManager;
class SpriteCommon;
class Object3dCommon;

//各シーンで共有するもの
struct SceneContext
{
    SpriteCommon* spriteCommon;
    Object3dCommon* object3dCommon;
	CameraManager* cameraManager;
	LightManager* lightManager;
	LineManager* linesManager;
};