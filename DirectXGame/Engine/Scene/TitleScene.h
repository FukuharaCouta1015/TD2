#pragma once
#include "MyMath.h"
#include "kamataEngine.h"
#include <vector>
#include "Fade.h"

class TitleScene {
public:

	// シーンのフェーズ
	enum class Phase {
		kMain,
		kFadeIn,
		kFadeOut,

	};

	// デストラクタ
	~TitleScene();
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	bool IsFinished() const { return finished_; }


	private:
	uint32_t textureHandle_ = 0; // テクスチャハンドル
	uint32_t gameoverHandle_ = 0;
	KamataEngine::Sprite* tdTitle_ = nullptr;
	KamataEngine::Sprite* gameover_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Camera camera_ ;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformPlayer_;
	float rotate = 0.0f;    // 回転角度
	bool finished_ = false; // タイトルシーンが終了したかどうか
	Fade* fade_ = nullptr;  // フェードオブジェクト
	Phase phase_ = Phase::kFadeIn; // シーンのフェーズ
};