#include "Mino.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "WorldTransform.h"

using namespace KamataEngine;

// 初期化
void Mino::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

// 更新
void Mino::Update() {
	for (Mino* mino : minos_) {
		// 移動。落下処理もこの中で書く
		mino->Move();

		// ここに下に落ち切ったミノをMapChipに書き込み、新しいミノを生成する関数を書く。CheckCollisionをその関数の中に移動させてもいい

		// worldTransformを転送する
		WorldTransformUpdate(mino->worldTransform_);
	}
}

// 描画
void Mino::Draw() {
	for (Mino* mino : minos_) {
		mino->model_->Draw(mino->worldTransform_, *mino->camera_);
	}
}

// 移動
void Mino::Move() {
	// フレームをカウントする
	++frameCount;

	// 左右移動
	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		worldTransform_.translation_.x -= 1;
	} else if (Input::GetInstance()->TriggerKey(DIK_D)) {
		worldTransform_.translation_.x += 1;
	}

	// 落下処理。20フレーム毎に落下する
	if (frameCount % 20 == 19) {
		worldTransform_.translation_.y -= 1;
	}

	// 落下加速
	/*if (Input::GetInstance()->PushKey(DIK_S)) {
	    worldTransform_.translation_.y -= 1;
	}*/
}

// 当たり判定
bool Mino::CheckCollision() { 
	return false;
}

// ミノを生成
void Mino::GenerateMino(MinoType minoType, Model* model, Camera* camera) {
	Vector3 minoPos[4];

	// MinoType毎に座標を設定
	switch (minoType) {
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
		minos_.push_back(newMino);
	}
}
