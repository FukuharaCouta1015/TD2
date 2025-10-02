#pragma once
#include <kamataEngine.h>
#include <vector>
#include "Player.h"
#include "Skydome.h"
#include <map>
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include "Fade.h"

class GameScene {
public:

	~GameScene();
	// 初期化

	std::vector <std::vector<KamataEngine::WorldTransform*>> WorldTransformBlocks_;

	bool isDebugCameraActive_ = false;

	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	void GenerateBlocks();

	void CheckAllCollision();

	void ChangePhase();

	KamataEngine::Sprite* sprite_ = nullptr;

	//uint32_t textureHandle_ = 0;	

	MapChipField* mapChipField_ = nullptr;

	bool IsFinished() const { return finished_; }
	


	// モデル	
	KamataEngine::Model* model_ = nullptr;
	//モデルエネミー
	KamataEngine::Model* modelEnemy_ = nullptr;
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
	// プレイヤー
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;
	std::list<Enemy*> enemies_;
	CameraController* cameraController_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* modelDeathParticles_ = nullptr;
	bool finished_ = false; // ゲームシーンが終了したかどうか
	Fade* fade_ = nullptr;  // フェードオブジェクト

};