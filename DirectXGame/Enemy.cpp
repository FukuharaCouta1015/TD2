//#include "Enemy.h"
//#include <cassert>
//#include "kamataEngine.h"
//#include "MyMath.h"
//#include <numbers>
//
//using namespace KamataEngine;
//using namespace MathUtility;
//
//void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
//
//	assert(model);
//	model_ = model;
//	worldTransform_.Initialize();
//	worldTransform_.translation_ = position;
//	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f; // 左向きに設定
//	camera_ = camera;
//	velocity_ = {-kWalkSpeed, 0, 0}; // スケールを設定
//	walkTimer_ = 0.0f;               // 歩行モーションのタイマーを初期化
//	
//}
//
//void Enemy::Update() {
//	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
//	worldTransform_.TransferMatrix();
//	worldTransform_.translation_ += velocity_; // 位置を更新
//	walkTimer_ += 1.0f / 60.0f;  // タイマーを更新
//	worldTransform_.rotation_.x = std::sin(walkTimer_ * 5.0f);
//}
//
//void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }
//
//AABB Enemy::GetAABB() {
//	Vector3 worldPos = GetWorldPosition();
//	AABB aabb;
//	aabb.min = {worldPos.x - 1.0f / 2.0f, worldPos.y - 1.0f / 2.0f, worldPos.z - 1.0f / 2.0f};
//	aabb.max = {worldPos.x + 1.0f / 2.0f, worldPos.y + 1.0f / 2.0f, worldPos.z + 1.0f / 2.0f};
//
//	return aabb;
//}
//
//KamataEngine::Vector3 Enemy::GetWorldPosition() {
//	Vector3 worldPos;
//	worldPos.x = worldTransform_.matWorld_.m[3][0];
//	worldPos.y = worldTransform_.matWorld_.m[3][1];
//	worldPos.z = worldTransform_.matWorld_.m[3][2];
//	return worldPos;
//}
//
//void Enemy::OnCollision(const Player* player) { 
//	(void)player; 
//}
