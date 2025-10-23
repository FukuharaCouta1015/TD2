#pragma once
#include "kamataEngine.h"
#include <vector>

enum class MinoType { I, O, S, Z, J, L, T };

class MapChipField;

class Mino {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ミノを生成
	void GenerateMino(MinoType minoType, KamataEngine::Model* model, KamataEngine::Camera* camera);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:
	// 移動
	void Move();
	// 当たり判定
	bool CheckCollision();

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_;
	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// ミノのリスト
	std::list<Mino*> minos_;

	// フレームカウント
	uint32_t frameCount = 0;
};