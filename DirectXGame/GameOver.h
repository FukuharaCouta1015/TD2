#pragma once

#include "Fade.h"
#include "MyMath.h"
#include "kamataEngine.h"
#include <vector>

class GameOverScene {
public:
	// シーンのフェーズ
	enum class Phase {
		kMain,
		kFadeIn,
		kFadeOut,
	};

	// デストラクタ
	~GameOverScene();
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	
	uint32_t gameOverHandle_ = 0;

	KamataEngine::Sprite* gameOver_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	
	KamataEngine::Camera camera_;
	KamataEngine::WorldTransform worldTransform_;
	float rotate = 0.0f;           // 回転角度
	bool finished_ = false;        // タイトルシーンが終了したかどうか
	Fade* fade_ = nullptr;         // フェードオブジェクト
	Phase phase_ = Phase::kFadeIn; // シーンのフェーズ
};