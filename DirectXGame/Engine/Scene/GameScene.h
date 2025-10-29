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
	// コンストラクタ、デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// ゲームシーンが終了したかどうかを取得
	bool IsFinished() const { return finished_; }
    // ゲームがオーバー状態であるかを取得
    bool IsGameOver() const { return isGameOver_; }
    // ゲームオーバー状態を設定
    void SetGameOver(bool gameOver) { isGameOver_ = gameOver; }

private:
	// フェーズ管理
	enum class Phase {
		kPlay,    // プレイ中
		kDeath,   // 死亡時
		kFadeIn,  // フェードイン中
		kFadeOut, // フェードアウト中
	};
	Phase phase_;

	// ゲームシーンが終了したかどうか
	bool finished_ = false; 
    // ゲームがオーバー状態であるか
    bool isGameOver_ = false; 

	// デバッグカメラがアクティブかどうか
	bool isDebugCameraActive_ = false;

	// オブジェクト関連
	KamataEngine::Model* model_ = nullptr;        // 汎用モデル（例: プレイヤー）
	KamataEngine::Model* modelBlock_ = nullptr;   // ブロックモデル
	KamataEngine::Model* modelSkydome_ = nullptr; // 天球モデル
	Skydome* skydome_ = nullptr;                  // スカイドームオブジェクト
	Mino* mino_ = nullptr;                        // ミノオブジェクト
	MapChipField* mapChipField_ = nullptr;        // マップチップフィールド

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// ワールドトランスフォームのブロックリスト
	std::vector<std::vector<KamataEngine::WorldTransform*>> WorldTransformBlocks_;

	// カメラ関連
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// UI/エフェクト関連
	Fade* fade_ = nullptr;  // フェードオブジェクト
	KamataEngine::Sprite* sprite_ = nullptr; // 汎用スプライト（現在のコードでは未使用？）
	uint32_t textureHandle_ = 0;             // 汎用テクスチャハンドル（現在のコードでは未使用？）

	// ミノ
	uint32_t textureHandleMino_ = KamataEngine::TextureManager::Load("./resources/mino.png");

	// スプライト（左右移動ボタン）
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

	// 内部処理関数
	void GenerateBlocks();   // ブロックの生成
	void CheckAllCollision(); // 全ての衝突判定
	void ChangePhase();      // フェーズの変更
	// 追加: 回転スプライトの判定（クリックでミノ回転を要求）
	bool ProcessSpriteClickRotate(const KamataEngine::Vector3& spriteLeftTop, const KamataEngine::Vector3& spriteSize);
};