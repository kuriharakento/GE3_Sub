#pragma once
#include <unordered_map>
#include <vector>

#include "math/Quaternion.h"
#include "math/Vector3.h"

template <typename tValue>
struct Keyframe
{
	float time;
	tValue value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template<typename tValue>
struct AnimationCurve
{
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation
{
	AnimationCurve<Vector3> position;
	AnimationCurve<Quaternion> rotation;
	AnimationCurve<Vector3> scale;
};

struct Animation
{
	float duration;
	std::unordered_map<std::string, NodeAnimation> nodeAnimations;
};

inline Animation LoadAnimationFile(const std::string& filePath)
{

}
