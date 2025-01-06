#include "application/Animation/Slide.h"
#include <algorithm>
#include <engine/manager/TextureManager.h>

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

void Slide::Initialize(SpriteCommon* spriteCommon) {
	/*----------------[ スプライトの初期化 ]------------------*/
	
	for (std::size_t i = 0; i < sprites_.size(); i++) {
		sprites_[i].sprite = std::make_unique<Sprite>();
		sprites_[i].sprite->Initialize(spriteCommon,filePath_);
		sprites_[i].sprite->SetSize(Vector2(1280.0f, 720.0f));
		sprites_[i].sprite->SetPosition(Vector2(1280.0f, 720.0f));
		sprites_[i].sprite->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void Slide::Update() {
	// 現在のスライドの進行具合
	float progress = 0.0f;

	switch (status_) {
	case Status::None:
		break;
	case Status::SlideInFromLeft:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			Finish();
		}
		// 進行具合を時間に関係なく0.0f～1.0fの範囲で取得
		progress = counter_ / duration_;
		sprites_[0].sprite->SetPosition(Vector2(EasingByAmout<float>(kSlideInStartPos_, kSlideDistance_, pEasingFunc_, progress), 0.0f));
		break;
	case Status::SlideOutFromLeft:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			Finish();
		}
		// 進行具合を時間に関係なく0.0f～1.0fの範囲で取得
		progress = counter_ / duration_;
		sprites_[0].sprite->SetPosition(Vector2(EasingByAmout<float>(kSlideOutStartPos_, kSlideDistance_, pEasingFunc_, progress), 0.0f));
		break;
	case Status::SlideInFromBothSides:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			Finish();
		}
		// 進行具合を時間に関係なく0.0f～1.0fの範囲で取得
		progress = counter_ / duration_;
		SlideInFromBothSides(progress);
		break;
	case Status::SlideOutFromBothSides:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			Finish();
		}
		// 進行具合を時間に関係なく0.0f～1.0fの範囲で取得
		progress = counter_ / duration_;
		SlideOutFromBothSides(progress);
		break;
	case Status::SlideInFromFourCorners:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			Finish();
		}
		progress = counter_ / duration_;
		SlideInFromFourCorners(progress);
		break;
	case Status::SlideOutFromFourCorners:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			Finish();
		}
		progress = counter_ / duration_;
		SlideOutFromFourCorners(progress);
		break;
	}

	for (auto& sprite : sprites_) {
		sprite.sprite->Update();
	}

	/*----------------[ ImGui ]------------------*/
//#ifdef _DEBUG
//
//	ImGui::Begin("Slide");
//	ImGui::DragFloat("EasingTime", &easingTime_,0.01f);
//#pragma region SlideButton
//	if (ImGui::Button("SlideInBothSides")) {
//		Start(Slide::Status::SlideInFromBothSides,easingTime_);
//	}
//	ImGui::SameLine();
//	if (ImGui::Button("SlideOutBothSides"))
//	{
//		Start(Slide::Status::SlideOutFromBothSides, easingTime_);
//	}
//
//	if (ImGui::Button("SlideInFromLeft")) {
//		Start(Slide::Status::SlideInFromLeft, easingTime_);
//	}
//	ImGui::SameLine();
//	if (ImGui::Button("SlideOutToLeft"))
//	{
//		Start(Slide::Status::SlideOutFromLeft, easingTime_);
//	}
//
//	if (ImGui::Button("SlideInFromFourCorners")) {
//		Start(Slide::Status::SlideInFromFourCorners, easingTime_);
//	}
//	ImGui::SameLine();
//	if (ImGui::Button("SlideOutFromFourCorners"))
//	{
//		Start(Slide::Status::SlideOutFromFourCorners, easingTime_);
//	}
//
//#pragma endregion
//	ImGui::Text("Status: %d", static_cast<int>(status_));
//	ImGui::Text("Duration: %f", duration_);
//	ImGui::Text("Counter: %f", counter_);
//	ImGui::Text("Finish: %s", isFinish_ ? "true" : "false");
//	for (std::size_t i = 0; i < sprites_.size(); i++) {
//		ImGui::Text("Sprite[%d]: %s", i, sprites_[i].isMove ? "true" : "false");
//		ImGui::Text("Sprite[%d] Position: (%f, %f)", i, sprites_[i].sprite->GetPosition().x, sprites_[i].sprite->GetPosition().y);
//	}
//	ImGui::End();
//#endif // DEBUG
}

void Slide::Draw() {
	std::for_each(sprites_.begin(), sprites_.end(), [&](SlideSprite& sprite) {
		if (sprite.isMove) {
			sprite.sprite->Draw();
		}
	});
}

void Slide::Start(Status status, float duration) {
	// スライドの変数の初期化
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
	isFinish_ = false;
	InitializeSprites();

	// 使うスプライトを区別する
	switch (status_) {
	case Status::SlideInFromLeft: // １枚だけ使う
		sprites_[0].isMove = true;
		sprites_[1].isMove = false;
		sprites_[2].isMove = false;
		sprites_[3].isMove = false;
		break;
	case Status::SlideOutFromLeft: // １枚だけ使う
		sprites_[0].isMove = true;
		sprites_[1].isMove = false;
		sprites_[2].isMove = false;
		sprites_[3].isMove = false;
		break;
	case Status::SlideInFromBothSides: // ２枚使う
		sprites_[0].isMove = true;
		sprites_[1].isMove = true;
		sprites_[2].isMove = false;
		sprites_[3].isMove = false;
		//画像をうまく切り取って半分ずつ表示する
		sprites_[0].sprite->SetTextureLeftTop(Vector2(0.0f,0.0f));
		sprites_[0].sprite->SetTextureSize(Vector2(640.0f, 720.0f));
		sprites_[0].sprite->SetSize(Vector2(640.0f, 720.0f));
		sprites_[1].sprite->SetTextureLeftTop(Vector2(640.0f, 0.0f));
		sprites_[1].sprite->SetTextureSize(Vector2(640.0f, 720.0f));
		sprites_[1].sprite->SetSize(Vector2(640.0f, 720.0f));
		break;
	case Status::SlideOutFromBothSides: // ２枚使う
		sprites_[0].isMove = true;
		sprites_[1].isMove = true;
		sprites_[2].isMove = false;
		sprites_[3].isMove = false;
		sprites_[0].sprite->SetTextureLeftTop(Vector2(0.0f, 0.0f));
		sprites_[0].sprite->SetTextureSize(Vector2(640.0f, 720.0f));
		sprites_[0].sprite->SetSize(Vector2(640.0f, 720.0f));
		sprites_[1].sprite->SetTextureLeftTop(Vector2(640.0f, 0.0f));
		sprites_[1].sprite->SetTextureSize(Vector2(640.0f, 720.0f));
		sprites_[1].sprite->SetSize(Vector2(640.0f, 720.0f));
		break;
	case Status::SlideInFromFourCorners: // ４枚使う
		sprites_[0].isMove = true;
		sprites_[1].isMove = true;
		sprites_[2].isMove = true;
		sprites_[3].isMove = true;
		break;
	case Status::SlideOutFromFourCorners: // ４枚使う
		sprites_[0].isMove = true;
		sprites_[1].isMove = true;
		sprites_[2].isMove = true;
		sprites_[3].isMove = true;
		break;
	}
}

void Slide::InitializeSprites() {
	// スプライトの初期化
	for (std::size_t i = 0; i < sprites_.size(); i++) {
		sprites_[i].sprite->SetPosition(Vector2(1280.0f, 720.0f));
		sprites_[i].isMove = false;
	}
}

void Slide::Finish() {
	counter_ = duration_;
	status_ = Status::None;
	// 終了フラグを立てる
	isFinish_ = true;
}

void Slide::SlideInFromBothSides(float progress) {
	// 左からスライドイン
	sprites_[0].sprite->SetPosition(Vector2(EasingByAmout(kSlideInBothSidesStartPos_.left, kSlideBothSidesDistance_, pEasingFunc_, progress), 0.0f));
	// 右からスライドイン
	sprites_[1].sprite->SetPosition(Vector2(EasingByAmout(kSlideInBothSidesStartPos_.right, -kSlideBothSidesDistance_, pEasingFunc_, progress), 0.0f));
}

void Slide::SlideOutFromBothSides(float progress) {
	// 左へスライドアウト
	sprites_[0].sprite->SetPosition(Vector2(EasingByAmout(kSlideOutBothSidesStartPos_.left, -kSlideBothSidesDistance_, pEasingFunc_, progress), 0.0f));
	// 右へスライドアウト
	sprites_[1].sprite->SetPosition(Vector2(EasingByAmout(kSlideOutBothSidesStartPos_.right, kSlideBothSidesDistance_, pEasingFunc_, progress), 0.0f));
}

void Slide::SlideInFromFourCorners(float progress) {
	// 左上
	sprites_[0].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideInFourCornersStartPos_.left, kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideInFourCornersStartPos_.top, kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
	// 右上
	sprites_[1].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideInFourCornersStartPos_.right, -kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideInFourCornersStartPos_.top, kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
	// 左下
	sprites_[2].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideInFourCornersStartPos_.left, kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideInFourCornersStartPos_.bottom, -kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
	// 右下
	sprites_[3].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideInFourCornersStartPos_.right, -kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideInFourCornersStartPos_.bottom, -kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
}

void Slide::SlideOutFromFourCorners(float progress) {
	// 左上
	sprites_[0].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideOutFourCornersStartPos_.left, -kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideOutFourCornersStartPos_.top, -kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
	// 右上
	sprites_[1].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideOutFourCornersStartPos_.right, kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideOutFourCornersStartPos_.top, -kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
	// 左下
	sprites_[2].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideOutFourCornersStartPos_.left, -kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideOutFourCornersStartPos_.bottom, kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
	// 右下
	sprites_[3].sprite->SetPosition(Vector2(
	    EasingByAmout(kSlideOutFourCornersStartPos_.right, kSlideFourCornersDistance_.x, pEasingFunc_, progress),
	    EasingByAmout(kSlideOutFourCornersStartPos_.bottom, kSlideFourCornersDistance_.y, pEasingFunc_, progress)));
}
