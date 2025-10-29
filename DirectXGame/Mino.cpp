#include "Mino.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "WorldTransform.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <random>

using namespace KamataEngine;

// 乱数エンジンをファイルスコープで一度だけ作成（GenerateMino 内で再作成しない）
namespace {
static std::random_device rd;
static std::mt19937 gen(rd());

// 7-Bag 用の袋（0..6 をシャッフルして順に取り出す）
static std::array<int, 7> bag;
static int bagIndex = 7; // 7 のときは「袋が空」、補充が必要

// 直前に生成したミノの種類を記憶（袋の補充時に境界重複を防ぐために使う）
static bool hasPrevMinoType = false;
static MinoType prevMinoType = MinoType::I;

// 袋を補充してシャッフル（補充後、袋[0] が直前と同じなら交換して防ぐ）
static void RefillBag() {
	for (int i = 0; i < 7; ++i) {
		bag[i] = i;
	}
	std::shuffle(bag.begin(), bag.end(), gen);

	if (hasPrevMinoType && bag[0] == static_cast<int>(prevMinoType)) {
		// bag[0] と異なる要素を探して交換
		for (int i = 1; i < 7; ++i) {
			if (bag[i] != static_cast<int>(prevMinoType)) {
				std::swap(bag[0], bag[i]);
				break;
			}
		}
		// 万が一全要素が同じ（理論上ありえない）はそのまま
	}
	bagIndex = 0;
}
} // namespace

// 初期化
void Mino::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);
	model_ = model;
	camera_ = camera;
	// 生成元として保持（親用）
	prototypeModel_ = model;
	prototypeCamera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 前フレーム位置を初期化
	prevTranslation_ = worldTransform_.translation_;
}

// 外部からの移動要求（dx: -1 左, +1 右）
void Mino::RequestMove(int dx) { moveRequest_ = dx; }

// 更新
void Mino::Update() {
	// 親インスタンスが管理するミノ群の移動はここで一括制御する
	// フレームカウントは親で管理
	++frameCount;

	// 入力での横移動を決定（-1:左, +1:右, 0:なし）
	int dx = 0;
	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		dx = -1;
	} else if (Input::GetInstance()->TriggerKey(DIK_D)) {
		dx = +1;
	}

	// 外部からの移動要求があれば優先して適用
	if (moveRequest_ != 0) {
		dx = moveRequest_;
		moveRequest_ = 0;
	}

	// 落下（-1:下へ1マス） - 親のフレームで判定
	int dy = 0;
	if (frameCount % 15 == 14) {
		dy = -1;
	}

	// マップ参照が無ければ何もしない（安全）
	if (!mapChipField_) {
		for (Mino* mino : minos_) {
			WorldTransformUpdate(mino->worldTransform_);
		}
		return;
	}

	// 横移動が壁に当たるか事前チェックする（任意のミノが当たれば全体で移動しない）
	if (dx != 0) {
		bool blocked = false;
		for (Mino* mino : minos_) {
			Vector3 target = mino->worldTransform_.translation_;
			target.x += dx;
			auto idx = mapChipField_->GetMapChipIndexByPosition(target);
			if (mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kBlock || mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kMino) {
				blocked = true;
				break;
			}
		}
		if (blocked) {
			dx = 0; // 横移動をキャンセル
		}
	}

	// 各ミノの前フレーム位置を保存（めり込み解決に使う）
	for (Mino* mino : minos_) {
		mino->prevTranslation_ = mino->worldTransform_.translation_;
	}

	// 全ミノに移動を適用
	for (Mino* mino : minos_) {
		if (dx != 0)
			mino->worldTransform_.translation_.x += dx;
		if (dy != 0)
			mino->worldTransform_.translation_.y += dy;
	}

	// 衝突判定（着地検出）。どれか一つでも「下方向から当たった（着地）」なら全固定
	bool anyLanded = false;
	for (Mino* mino : minos_) {
		auto idx = mapChipField_->GetMapChipIndexByPosition(mino->worldTransform_.translation_);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
		if (type == MapChipType::kBlock || type == MapChipType::kMino) {
			// 以前の位置と比べて下方向に移動して当たっていれば着地とみなす
			if (mino->prevTranslation_.y > mino->worldTransform_.translation_.y) {
				anyLanded = true;
				break;
			} else {
				// 上や左右のめり込みは元の位置へ戻す
				mino->worldTransform_.translation_ = mino->prevTranslation_;
			}
		}
	}

	if (anyLanded) {
		// 着地判定があった場合、"めり込む前の位置(prevTranslation_)" にブロックを書き込む（＝着地前の位置で固定）
		for (Mino* mino : minos_) {
			auto idxPrev = mapChipField_->GetMapChipIndexByPosition(mino->prevTranslation_);
			uint32_t x = idxPrev.xIndex;
			uint32_t y = idxPrev.yIndex;
			// 範囲チェックして書き込み
			if (x < mapChipField_->GetNumBlockHorizontal() && y < mapChipField_->GetNumBlockVertical()) {
				mapChipField_->mapChipData_.data_[y][x] = MapChipType::kMino;
			}
		}

		// 子ミノを解放（子の model_/camera_ をクリアしない）
		for (Mino* mino : minos_) {
			delete mino;
		}
		minos_.clear();

		// 固定されたら次のミノを生成する
		// GenerateMino 内で prototype を利用/更新するため、ここでは引数無しで呼ぶ
		GenerateMino();

		return;
	}

	// 衝突や着地が無ければ残ったミノをワールド転送して描画可能にする
	for (Mino* mino : minos_) {
		WorldTransformUpdate(mino->worldTransform_);
	}
}

// 描画
void Mino::Draw() {
	for (Mino* mino : minos_) {
		// 子の model_/camera_ は生成時に渡した共有ポインタ（所有せず使い回す）
		if (mino->model_ && mino->camera_) {
			mino->model_->Draw(mino->worldTransform_, *mino->camera_);
		}
	}
}

// ミノを生成
void Mino::GenerateMino(Model* model, Camera* camera) {
	// 明示的に model/camera が渡された場合、それをプロトタイプとして保存する
	if (model && camera) {
		prototypeModel_ = model;
		prototypeCamera_ = camera;
	}

	// 引数が nullptr の場合は prototype を使う
	if (!model)
		model = prototypeModel_;
	if (!camera)
		camera = prototypeCamera_;

	// 最終的にモデル/カメラが無ければ原因を分かりやすくするためアサート
	assert(model && camera);

	// 7-Bag から取り出す（袋が空なら補充）
	if (bagIndex >= 7) {
		RefillBag();
	}
	int candidate = bag[bagIndex++];
	minoType_ = static_cast<MinoType>(candidate);

	// 直前種類を更新
	prevMinoType = minoType_;
	hasPrevMinoType = true;

	Vector3 minoPos[4];

	// MinoType毎に座標を設定
	switch (minoType_) {
	case MinoType::I:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(4, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(5, 0);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(6, 0);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(7, 0);
		break;
	case MinoType::O:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(5, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(6, 0);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(5, 1);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(6, 1);
		break;
	case MinoType::S:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(5, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(6, 0);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(4, 1);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(5, 1);
		break;
	case MinoType::Z:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(4, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(5, 0);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(5, 1);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(6, 1);
		break;
	case MinoType::J:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(4, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(4, 1);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(5, 1);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(6, 1);
		break;
	case MinoType::L:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(6, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(4, 1);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(5, 1);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(6, 1);
		break;
	case MinoType::T:
		minoPos[0] = mapChipField_->GetMapChipPositionByIndex(5, 0);
		minoPos[1] = mapChipField_->GetMapChipPositionByIndex(4, 1);
		minoPos[2] = mapChipField_->GetMapChipPositionByIndex(5, 1);
		minoPos[3] = mapChipField_->GetMapChipPositionByIndex(6, 1);
		break;
	}

	// ミノを生成
	for (uint32_t i = 0; i < 4; ++i) {
		Mino* newMino = new Mino();
		newMino->Initialize(model, camera, minoPos[i]);
		// マップ参照を渡す
		newMino->SetMapChipField(mapChipField_);
		minos_.push_back(newMino);
	}
}