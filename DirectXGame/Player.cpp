#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"

#include "MyMath.h"
#include "kamataEngine.h"
#include <algorithm>
#include <cassert>
#include <numbers>
using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	camera_ = camera;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// 移動入力
	InputMove();

	// 移動量を加味して衝突判定する
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 判定結果を反映して移動する
	CheckMapMove(collisionMapInfo);

	// 天井に接している場合の処理
	CheckMapCeiling(collisionMapInfo);

	// 壁に接している場合の処理
	CheckMapWall(collisionMapInfo);

	// 地状態の切り替え
	CheckMapLanding(collisionMapInfo);

	// 旋回制御
	AnimateTurn();

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	if (isDead_ == true) {
		return;
	}

	model_->Draw(worldTransform_, *camera_);

}

// 移動
void Player::InputMove() {

    // 二段ジャンプ用フラグとカウントを追加
    static int jumpCount = 0;
    static const int kMaxJumpCount = 2;

    if (onGround_) {
        jumpCount = 0; // 地面にいるときはジャンプ回数リセット
        if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
            Vector3 acceleration = {};
            if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
                acceleration.x += kAcceleration; // 右方向の加速度
                if (velocity_.x > 0.0f) {
                    velocity_.x *= (1.0f - kAttenuation);
                }
                if (lrDirection_ != LRDirection::kLeft) {
                    lrDirection_ = LRDirection::kLeft; // 右方向に変更
                    turnFirstRotationY_ = worldTransform_.rotation_.y;
                    turnTimer_ = kTimeTurn;
                }
                velocity_ += acceleration; // 速度に加速度を加える
            } else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
                acceleration.x -= kAcceleration; // 左方向の加速度
                if (velocity_.x > 0.0f) {
                    velocity_.x *= (1.0f - kAttenuation);
                }
                if (lrDirection_ != LRDirection::kRight) {
                    lrDirection_ = LRDirection::kRight; // 左方向に変更
                    turnFirstRotationY_ = worldTransform_.rotation_.y;
                    turnTimer_ = kTimeTurn;
                }
                velocity_ += acceleration; // 速度に加速度を加える
                velocity_.x = std::clamp(velocity_.x, -1.0f, 1.0f);
            }

        } else {
            velocity_.x *= (1.0f - kAttenuation);

            velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
        }

        // ジャンプ
        if (Input::GetInstance()->PushKey(DIK_UP)) {
            if (jumpCount < kMaxJumpCount) {
                velocity_ += Vector3(0, kJmupAcceleration, 0);
                ++jumpCount;
                onGround_ = false; // 空中状態にする
            }
        }
    } else {
        velocity_ += Vector3(0, -kGravityAcceleration, 0);

        // 空中でジャンプ
        if (Input::GetInstance()->PushKey(DIK_UP)) {
            if (jumpCount < kMaxJumpCount) {
                velocity_.y = kJmupAcceleration; // 垂直速度を上書き
                ++jumpCount;
                onGround_ = false; // 空中状態にする
            }
        }
    }
}
// マップ衝突チェック
void Player::CheckMapCollision(CollisionMapInfo& Info) {
	CheckMapCollisionUp(Info);
	CheckMapCollisionDown(Info);
	CheckMapCollisionRight(Info);
	CheckMapCollisionLeft(Info);
}

// マップ衝突チェック(上)
void Player::CheckMapCollisionUp(CollisionMapInfo& Info) {

	// 上昇有り
	if (Info.move.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上のあたり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにhit?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Info.move + Vector3(0, +kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			Info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			// 天井にあったていることを記録する
			Info.ceiling = true;
		}
	}
}

// マップ衝突チェック(下)
void Player::CheckMapCollisionDown(CollisionMapInfo& Info) {

	// 下降有り
	if (Info.move.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下のあたり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにhit?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Info.move + Vector3(0, -kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			Info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 床にあったていることを記録する
			Info.landing = true;
		}
	}
}
// マップ衝突チェック(右)
void Player::CheckMapCollisionRight(CollisionMapInfo& Info) {

	// 右移動有り
	if (Info.move.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真右のあたり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにhit?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Info.move + Vector3(+kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		Info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		// 壁にあったていることを記録する
		Info.hitWall = true;
	}
}

// マップ衝突チェック(左)
void Player::CheckMapCollisionLeft(CollisionMapInfo& Info) {

	// 左移動有り
	if (Info.move.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真左のあたり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにhit?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Info.move + Vector3(-kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		Info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		// 壁にあったていることを記録する
		Info.hitWall = true;
	}
}

// 判定結果を反映
void Player::CheckMapMove(const CollisionMapInfo& Info) {

	// 移動
	worldTransform_.translation_ += Info.move;
}

// 天井に接している場合の処理
void Player::CheckMapCeiling(const CollisionMapInfo& Info) {

	if (Info.ceiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
}
// 壁に接している場合の処理
void Player::CheckMapWall(const CollisionMapInfo& Info) {
	// 壁接触による減速
	if (Info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

// 接地状態の切り替え
void Player::CheckMapLanding(const CollisionMapInfo& Info) {
	if (onGround_) {
		// 接地状態の処理

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定
			//   移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + Info.move, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下のあたり判定を行う
			bool hit = false;
			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			if (!hit) {
				onGround_ = false; // 接地状態を解除
			}
		}

	} else {
		// 空中状態の処理
		if (Info.landing) {
			// 着地状態に切り替える
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding); // 水平方向の速度を減衰
			velocity_.y = 0.0f;                          // 垂直速度をリセット
		}
	}
}

// 旋回制御
void Player::AnimateTurn() {
	if (turnTimer_ > 0.0f) {

		turnTimer_ -= 1.0f / 60.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // 右
		    std::numbers::pi_v<float> * 3.0f / 2.0f // 左
		};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaselnOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0},
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - Player::kWidth / 2.0f, worldPos.y - Player::kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + Player::kWidth / 2.0f, worldPos.y + Player::kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	isDead_ = true;
	
}
