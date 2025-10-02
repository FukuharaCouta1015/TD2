#pragma once
#include <kamataEngine.h>


class Skydome {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model_, KamataEngine::Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();

private:

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera* camera_;
	uint32_t textureHandle_ = 0u;

};
