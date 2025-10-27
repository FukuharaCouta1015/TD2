#pragma once

#include "kamataEngine.h"

/// <summary>
///アフィン変換
/// </summary>

KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3& scale, KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation);
float EaselnOut(float x1, float x2, float t);

struct AABB {
	KamataEngine::Vector3 min; // 最小座標
	KamataEngine::Vector3 max; // 最大座標
};

bool IsCollision(const AABB& aabb1, const AABB& aabb2);