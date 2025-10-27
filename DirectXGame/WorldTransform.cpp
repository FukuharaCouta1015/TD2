#include "WorldTransform.h"

using namespace KamataEngine;

void WorldTransformUpdate(WorldTransform& worldTransform) {
	// アフィン変換行列の作成
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	// 行列を定数バッファに転送
	worldTransform.TransferMatrix();
}
