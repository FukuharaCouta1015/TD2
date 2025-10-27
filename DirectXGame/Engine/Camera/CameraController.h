#pragma once
#include "kamataEngine.h"
#include "Player.h"
#include <vector>
class Player;
class CameraController {
	
	
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	KamataEngine::Vector3  targetPosition_;

	static inline const float kInterpolationRate = 1.0f; // 補間速度

	static inline const float kVelocityBias = 1.0f;

	

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	static inline const Rect targetMargin = {-9.0f, 9.0, -5.0f, 5.0f};

	Rect movebleArea_ = {0, 100, 0, 100};

	void SetMovebleArea(const Rect& area) { movebleArea_ = area; }

	private:

	KamataEngine::Camera camera_;


	Player* target_ = nullptr; 


};