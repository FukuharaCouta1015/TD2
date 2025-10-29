#pragma once
#include "kamataEngine.h"
#include <vector>

enum class MinoType { I, O, S, Z, J, L, T };

class MapChipField;
class GameScene;


class Mino {
public:
	~Mino() = default;

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ミノを生成
	void GenerateMino(KamataEngine::Model* model = nullptr, KamataEngine::Camera* camera = nullptr);
	// mapChipFieldをセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 外部からの移動要求（dx: -1 左, +1 右）
	void RequestMove(int dx);

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void Rotate();

private:

	// 当たり判定
	bool CheckCollision();

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// モデル（所有しない前提）
	KamataEngine::Model* model_ = nullptr;
	// カメラ（所有しない前提）
	KamataEngine::Camera* camera_ = nullptr;
	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	uint32_t textureHandleMino_ = KamataEngine::TextureManager::Load("./resources/mino.png");
	GameScene* gameScene_ = nullptr;
	// ミノのリスト（子ブロック）
	std::list<Mino*> minos_;
	// ミノのタイプ
	MinoType minoType_;

	// フレームカウント
	uint32_t frameCount = 0;

	// 前フレームの位置（衝突解決のために保持）
	KamataEngine::Vector3 prevTranslation_;

	// 親が保持する「生成用」モデルとカメラ（子削除時の誤クリア対策）
	KamataEngine::Model* prototypeModel_ = nullptr;
	KamataEngine::Camera* prototypeCamera_ = nullptr;
	// 回転中心のインデックス
	int rotationCenterIndex_ = -1;

	bool CheckCollision(const std::vector<KamataEngine::Vector3>& tentativeBlockPositions);


	// 外部からの移動要求（1フレームのみ適用）
	int moveRequest_ = 0;
};