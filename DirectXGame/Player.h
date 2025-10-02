#pragma once
#include "kamataEngine.h"
#include <vector>
#include "MyMath.h"

class MapChipField;

class Enemy;
class Player {


public:
	void Initialize(KamataEngine::Model* model_, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	AABB GetAABB();

	void OnCollision(const Enemy* enemy);
	// デスフラグのゲッター
	bool isDead() const { return isDead_; }

	KamataEngine::Vector3 GetWorldPosition();

private:
	enum class LRDirection { kLeft, kRight };

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_;
	uint32_t textureHandle_ = 0u;

	KamataEngine::Vector3 velocity_ = {}; // 速度

	static inline const float kAcceleration = 0.1f; // 加速度
	static inline const float kAttenuation = 0.1f;  // 減衰率
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.1f; // 重力
	static inline const float kLimitFallSpeed = 1.0f;      // 最大速度
	static inline const float kJmupAcceleration = 1.0f;    // 初速

	LRDirection lrDirection_ = LRDirection::kRight; // 左右の方向

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	bool onGround_ = true;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	struct CollisionMapInfo {
		bool ceiling = false; 
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move;
	};


	//移動入力
	void  InputMove();

	//マップ衝突判定
	void CheckMapCollision(CollisionMapInfo& Info);
	// マップ衝突判定(上)
	void CheckMapCollisionUp(CollisionMapInfo& Info);

	// マップ衝突判定(下)
	void CheckMapCollisionDown(CollisionMapInfo& Info);

	// マップ衝突判定(右)
	void CheckMapCollisionRight(CollisionMapInfo& Info);

	// マップ衝突判定(左)
	void CheckMapCollisionLeft(CollisionMapInfo& Info);

	// 判定結果を反映して移動させる
	void CheckMapMove(const CollisionMapInfo& Info);

	// 天井に接している場合の処理
	void CheckMapCeiling(const CollisionMapInfo& Info);

	// 壁に接触している場合の処理
	void CheckMapWall(const CollisionMapInfo& Info);

	// 接地状態の切り替え
	void CheckMapLanding(const CollisionMapInfo& Info);

	//旋回制御
	void AnimateTurn();

	//角
	enum Corner {
		kRightBottom, // みぎした
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		
		

		kNumCorner   //要素数
	};

	//指定した角の座標計算
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	//隙間
	static inline const float kBlank = 0.1f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.5f; // 着地時の速度減衰率

	// 微小な数値
	static inline const float kGroundSearchHeight = 0.1f; 

	//壁の速度減衰率
	static inline const float kAttenuationWall = 0.5f; 

	

	//デスフラグ

	bool isDead_ = false;

	
	
};
