#include "DeathParticles.h"
#include "MyMath.h"
#include <cassert>
#include <numbers>
#include <algorithm>

using namespace KamataEngine;
using namespace MathUtility;


void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f}; // 白色で初期化

	for (KamataEngine::WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
	
}

void DeathParticles::Update() {


	for (KamataEngine::WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix();
	}

	for (uint32_t i = 0; i < 8; ++i) {
		Vector3 velocity = {kSpeed, 0, 0};
		float angle = kAngleUnit * i; // パーティクルの角度
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);// Y軸回転行列
		velocity = Transform(velocity, matrixRotation);  // 速度を回転させる
		worldTransforms_[i].translation_ += velocity;       // パーティクルの位置を更新
	}



	counter_ += 1.0f / 60.0f; // カウンターを更新

	color_.w = std::clamp(1.0f-counter_, 0.0f, 1.0f);

	objectColor_.SetColor(color_);

	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true; // パーティクルの終了フラグを立てる
	}

		if (isFinished_) {
		return; // パーティクルが終了している場合は更新をスキップ
	}
}

void DeathParticles::Draw() {
	for (const KamataEngine::WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_,&objectColor_);		
	}

	if (isFinished_) {
		return;
	}

}
