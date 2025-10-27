#include "CameraController.h"
#include "Player.h"
#include "Skydome.h"
#include "kamataEngine.h"
#include "MyMath.h"
#include "GameScene.h"

using namespace KamataEngine;
using namespace MathUtility;

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Update() {
	const KamataEngine::WorldTransform& targetworldTransform = target_->GetWorldTransform(); 
	const KamataEngine::Vector3& targetVelocity = target_->GetVelocity();
	targetPosition_ = targetworldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);

	camera_.translation_.x = max(camera_.translation_.x, camera_.translation_.x + targetMargin.left);
	camera_.translation_.x = min(camera_.translation_.x, camera_.translation_.x + targetMargin.right);
	camera_.translation_.y = max(camera_.translation_.y, camera_.translation_.y + targetMargin.bottom);
	camera_.translation_.y = min(camera_.translation_.y, camera_.translation_.y + targetMargin.top);



	//移動範囲制限
	camera_.translation_.x = max(camera_.translation_.x, movebleArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movebleArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movebleArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movebleArea_.top);


	camera_.UpdateMatrix();
}

void CameraController::Reset() { 
	const KamataEngine::WorldTransform& targetworldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetworldTransform.translation_ + targetOffset_;

}
