#pragma once
#include <kamataEngine.h>
#include <vector>
#include "Skydome.h"
#include <map>
#include "MapChipField.h"
#include "Fade.h"

class Mino;

class GameScene {
public:

	~GameScene();
	// 初期化

	std::vector <std::vector<KamataEngine::WorldTransform*>> WorldTransformBlocks_;

	bool isDebugCameraActive_ = true;

	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// ブロックの生成
	void GenerateBlocks();

	void CheckAllCollision();

	void ChangePhase();

	KamataEngine::Sprite* sprite_ = nullptr;

	//uint32_t textureHandle_ = 0;	

	MapChipField* mapChipField_ = nullptr;

	bool IsFinished() const { return finished_; }

	// モデル
	KamataEngine::Model* model_ = nullptr;
	//ブロック
	KamataEngine::Model* modelBlock_ = nullptr;	
	//天球
	KamataEngine::Model* modelSkydome_ = nullptr;
	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;	

	
	enum class Phase {
		kPlay,
		kDeath,
		kFadeIn,
		kFadeOut,
	};

	Phase phase_;

private:
	uint32_t textureHandle_ = 0;
	Skydome* skydome_ = nullptr;
	bool finished_ = false; // ゲームシーンが終了したかどうか
	Fade* fade_ = nullptr;  // フェードオブジェクト

	// ミノ
	Mino* mino_;
};