//#pragma once
//#include "kamataEngine.h"
//#include "MyMath.h"
//
//class Player;
//class Enemy {
//public:
//	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
//	// 更新
//	void Update();
//	// 描画
//	void Draw();
//
//	AABB GetAABB();
//
//	KamataEngine::Vector3 GetWorldPosition();
//
//	void OnCollision(const Player* player);
//
//private:
//
//	// モデル
//	KamataEngine::Model* model_ = nullptr;
//
//	// ワールドトランスフォーム
//	KamataEngine::WorldTransform worldTransform_;
//
//
//	// カメラ
//	KamataEngine::Camera* camera_;
//
//
//	static inline const float kWalkSpeed = 0.1f; // 移動速度
//    // Enemyクラスに敵の位置をx+20するメソッドを追加
//    public:
//       
//	static inline const float kWalkMotionAngleStart = 0.1f;
//
//	static inline const float kWalkMotionAngleEnd = 0.2f;
//
//	static inline const float kWalkMotionTime = 0.5f; 
//
//	float walkTimer_ = 0.0f; // 歩行モーションのタイマー
//};
