#pragma once
#include <memory>
#include <array>
#include "engine/math/Easing.h"
#include "engine/2d/Sprite.h"

class Slide {
public:
    enum class Status {
        None,                        // スライドなし
        SlideInFromLeft,             // 左からスライドイン
        SlideOutFromLeft,            // 左へスライドアウト
        SlideInFromBothSides,        // 両サイドからスライドイン
        SlideOutFromBothSides,       // 両サイドへスライドアウト
        SlideInFromFourCorners,      // 四つ角からスライドイン
        SlideOutFromFourCorners,     // 四つ角へスライドアウト
        SlideInBothSideSpritesWithDelay, // 遅延してスライドイン
        SideOutBothSIdeSpritesWithDelay  // 遅延してスライドアウト
    };

    void Initialize(SpriteCommon* spriteCommon);
    void Update();
    void Draw();
    void Start(Status status, float duration);

    void SetEasingFunc(float (*pEasingFunc)(float)) { pEasingFunc_ = pEasingFunc; }
    float (*GetEasingFunc() const)(float) { return pEasingFunc_; }
    bool IsFinish() const { return isFinish_; }
    float GetCounter() const { return counter_; }

private:
    void InitializeSprites();
    void Finish();
    void SlideInFromBothSides(float progress);
    void SlideOutFromBothSides(float progress);
    void SlideInFromFourCorners(float progress);
    void SlideOutFromFourCorners(float progress);
    void SlideInBothSideSpritesWithDelay(float progress);

    struct SlideSprite {
        std::unique_ptr<Sprite> sprite = nullptr;
        bool isMove = false;
        float timingOffset = 0.0f;
    };

    struct Direction {
        const float right;
        const float left;
        const float top;
        const float bottom;

        Direction(float right, float left, float top, float bottom) : right(right), left(left), top(top), bottom(bottom) {}
    };

    std::array<SlideSprite, 4> sprites_;
    std::array<SlideSprite, 6> fadeSprites_;

    const std::string kDebugPngPath = "./Resources/testSprite.png";
    const std::string kBlackPngPath = "./Resources/black.png";

    Status status_ = Status::None;
    bool isFinish_ = false;
    float duration_ = 0.0f;
    float counter_ = 0.0f;
    float (*pEasingFunc_)(float) = EaseInSine;
    float easingTime_ = 1.0f;

    const float kSlideDistance_ = 1280.0f;
    const float kSlideInStartPos_ = -1280.0f;
    const float kSlideOutStartPos_ = 0.0f;

    Direction kSlideInBothSidesStartPos_ = { 1280.0f, -640.0f, 0.0f, 0.0f };
    Direction kSlideOutBothSidesStartPos_ = { 640.0f, 0.0f, 0.0f, 0.0f };
    const float kSlideBothSidesDistance_ = 640.0f;

    Direction kSlideInFourCornersStartPos_ = { 1280.0f, -640.0f, -360.0f, 720.0f };
    Direction kSlideOutFourCornersStartPos_ = { 640.0f, 0.0f, 0.0f, 360.0f };
    const Vector2 kSlideFourCornersDistance_ = { 640.0f, 360.0f };

    Direction kSlideInBothSidesWithDelayStartPos_ = { 1280.0f, -1280.0f, 0.0f, 0.0f };
    const float kSlideBothSidesWithDelayDistance_ = 640.0f;
};
