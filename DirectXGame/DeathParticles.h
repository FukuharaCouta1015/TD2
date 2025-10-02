#pragma once
#include "kamataEngine.h"
#include <array>
#include <numbers>

class DeathParticles {

public:
	void Initialize(KamataEngine::Model* model_, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	bool IsFinished() const { return isFinished_; } // パーティクルが終了したかどうかを返す



private:

	static inline const uint32_t kNumParticles = 8; 

	static inline const float kDuration = 1.0f;

	static inline const float kSpeed = 0.1f;

	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float>/ kNumParticles; 

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_; // パーティクルの位置

	bool isFinished_ = false; // パーティクルの終了フラグ

	float counter_ = 0.0f; // カウンター

	KamataEngine::ObjectColor objectColor_;

	KamataEngine::Vector4 color_;
	
	// モデル
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_;



};
