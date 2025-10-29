#pragma once
#include "Fade.h"
#include "MapChipField.h"
#include "Skydome.h"
#include <kamataEngine.h>
#include <map>
#include <vector>

class Mino;

class GameScene {
public:
	~GameScene();
	// 初期化

	std::vector<std::vector<KamataEngine::WorldTransform*>> WorldTransformBlocks_;

	bool isDebugCameraActive_ = false;

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

	// uint32_t textureHandle_ = 0;

	MapChipField* mapChipField_ = nullptr;

	bool IsFinished() const { return finished_; }

	// モデル
	KamataEngine::Model* model_ = nullptr;
	// ブロック
	KamataEngine::Model* modelBlock_ = nullptr;
	// ミノ
	KamataEngine::Model* modelMino_ = nullptr;
	// 天球
	KamataEngine::Model* modelSkydome_ = nullptr;
	// ワールドトランスフォーム
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
	Mino* mino_ = nullptr;
	uint32_t textureHandleMino_ = KamataEngine::TextureManager::Load("./resources/mino.png");

	uint32_t textureHandleRight_ = KamataEngine::TextureManager::Load("./resources/arrow/right.png");
	uint32_t textureHandleLeft_ = KamataEngine::TextureManager::Load("./resources/arrow/left.png");
	// 追加: 回転ボタン用テクスチャ
	uint32_t textureHandleRotate_ = KamataEngine::TextureManager::Load("./resources/arrow/rotate.png");

	// スプライト（左右2つ + 上1つ）
	KamataEngine::Sprite* spriteRight_ = nullptr;  // 右移動トリガー（右側に表示するもの）
	KamataEngine::Sprite* spriteLeft_ = nullptr;   // 左移動トリガー（左隣に表示するもの）
	KamataEngine::Sprite* spriteRotate_ = nullptr; // 回転トリガー（上に表示するもの）

	// スプライトの左上位置（ピクセル）とサイズ（ピクセル）を保持（左上アンカー）
	KamataEngine::Vector3 spriteRightPos_ = {300.0f, 300.0f, 0.0f};
	KamataEngine::Vector3 spriteLeftPos_ = {80.0f, 300.0f, 0.0f}; // 初期は右の左隣に配置
	// 追加: 回転ボタンの左上位置
	KamataEngine::Vector3 spriteRotatePos_ = {300.0f, 140.0f, 0.0f};

	KamataEngine::Vector3 spriteSize_ = {150.0f, 150.0f, 0.0f};

	// 当たり判定と移動要求送信を行う汎用関数
	// dx: -1 = 左移動要求, +1 = 右移動要求
	bool ProcessSpriteClickMove(const KamataEngine::Vector3& spriteLeftTop, const KamataEngine::Vector3& spriteSize, int dx);
	// 追加: 回転スプライトの判定（クリックでミノ回転を要求）
	bool ProcessSpriteClickRotate(const KamataEngine::Vector3& spriteLeftTop, const KamataEngine::Vector3& spriteSize);
};