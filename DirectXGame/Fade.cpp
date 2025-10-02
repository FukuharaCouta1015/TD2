#include "Fade.h"
#include <algorithm>
using namespace KamataEngine;


//初期化
void Fade::Initialize() {

	textureHandle_ = TextureManager::Load("white1x1.png");
	sprite_ = Sprite::Create(textureHandle_, {0, 0});
	sprite_->SetSize(Vector2(1280,720));
	sprite_->SetColor(Vector4(0,0, 0,1)); // 初期は透明


}

// 更新
void Fade::Update() { 
	switch (status_) {
	case State::None:
		// 何もしない
		break;
	case State::FadeIn:
		counter_ += 1.0f / 60.0f; // タイマーを更新

		if (counter_ >= duration_) {
			counter_ = duration_; // タイマーを持続時間に制限
		}
		sprite_->SetColor(Vector4(0, 0, 0,1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f))); // 色を更新
		break;
	case State::FadeOut:
		counter_ += 1.0f / 60.0f; // タイマーを更新

		if (counter_ >= duration_) {
			counter_ = duration_;  // タイマーを持続時間に制限
		}
		sprite_->SetColor(Vector4(0, 0, 0,std::clamp(counter_/duration_,0.0f,1.0f))); // 色を更新
		break;
	}

}

// 描画
void Fade::Draw() { 

	if (status_ == State::None) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());	
	sprite_->Draw();
	Sprite::PostDraw();

}

void Fade::Start(State status, float duration) {

	status_ = status;     // フェードの状態を設定
	duration_ = duration; // フェードの持続時間を設定
	counter_ = 0.0f;      // タイマーをリセット
}

void Fade::Stop() {
	status_ = State::None;                  // フェードの状態をNoneに設定

}

bool Fade::IsFinished() const { 
	switch (status_) {
	case State::FadeIn:
	case State::FadeOut:
		if (counter_ >= duration_) {
			return true; // フェードが終了している
		} else {
			return false; // フェードがまだ続いている
		}
	
	}

	return true;
}
