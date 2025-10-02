#include "GameScene.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include "TitleScene.h"
#include <map>

using namespace KamataEngine;

// デストラクタ
GameScene::~GameScene() {
	delete player_;
	delete model_;
	delete debugCamera_;
	delete modelBlock_;
	delete modelSkydome_;
	delete skydome_;
	delete mapChipField_;
	delete cameraController_;
	delete deathParticles_;
	delete fade_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	delete modelEnemy_;

	for (std::vector<WorldTransform*>& WorldTransformBlockLine : WorldTransformBlocks_) {
		for (WorldTransform* WorldTransformBlock : WorldTransformBlockLine) {
			delete WorldTransformBlock;
		}
	}
	WorldTransformBlocks_.clear();
}

void GameScene::Initialize() {
	// 画像の初期化
	model_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelBlock_ = Model::CreateFromOBJ("block");

	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle");

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	debugCamera_ = new DebugCamera(1280, 720);

	mapChipField_ = new MapChipField();
	mapChipField_->LodeMapChipCsv("Resources/blocks.csv");

	// const uint32_t kNumBlocksHorizontal = 20;
	// const uint32_t kNumBlocksVertical = 10;

	// const float kBlockWidth = 2.0f;
	// const float kBlockHeight = 2.0f;

	GenerateBlocks();

	// WorldTransformBlocks_.resize(kNumBlocksVertical);
	// for (uint32_t i = 0; i < kNumBlocksVertical; ++i) {
	//	WorldTransformBlocks_[i].resize(kNumBlocksHorizontal);
	// }

	// ワールド
	worldTransform_.Initialize();

	// カメラ
	camera_.Initialize();

	// プレイヤー
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_->Initialize(model_, &camera_, playerPosition);

	// エネミー

	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(30 + i, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(40 + i*2, 16);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};

	cameraController_->SetMovebleArea(cameraArea);

	player_->SetMapChipField(mapChipField_);

	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelDeathParticles_, &camera_, playerPosition);

	phase_ = Phase::kPlay;

	fade_ = new Fade();
	fade_->Initialize();                     // フェードの初期化
	fade_->Start(Fade::State::FadeIn, 1.0f); // フェードインを開始
}

void GameScene::Update() {

	fade_->Update();

	switch (phase_) {
	case Phase::kPlay:
		if (player_->isDead()) {
			phase_ = Phase::kDeath;
			const Vector3& deadParticlesPosition = player_->GetWorldPosition();
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deadParticlesPosition); // デスパーティクルの初期化
		}
		break;
	case Phase::kDeath:

		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;
	case Phase::kFadeIn:
		fade_->Update();
		break;
	case Phase::kFadeOut:
		fade_->Update();
		break;
	}

	debugCamera_->Update();
	cameraController_->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // _DEBUG

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		camera_.TransferMatrix();
	}

	player_->Update();
	for (std::vector<WorldTransform*>& WorldTransformBlockLine : WorldTransformBlocks_) {
		for (WorldTransform* WorldTransformBlock : WorldTransformBlockLine) {
			if (!WorldTransformBlock) {
				continue;
			}
			WorldTransformBlock->matWorld_ = MakeAffineMatrix(WorldTransformBlock->scale_, WorldTransformBlock->rotation_, WorldTransformBlock->translation_);
			WorldTransformBlock->TransferMatrix();
		}
	}

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	CheckAllCollision();

	ChangePhase();
}

void GameScene::CheckAllCollision() {
#pragma region 自キャラと敵キャラのあたり判定
	AABB aabb1, aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		if (player_->isDead() == true) {
			phase_ = Phase::kDeath;
			const Vector3& deadParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deadParticlesPosition);
		}
		break;
	case Phase::kDeath:
		if (deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::State::FadeOut, 1.0f); // フェードインを開始
		}
		break;
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true; // ゲームシーンを終了
		}
		break;
	}
}

void GameScene::GenerateBlocks() {

	uint32_t numBlocksHorizontal = mapChipField_->GetNumBlockHorizontal();
	uint32_t numBlocksVertical = mapChipField_->GetNumBlockVertical();

	WorldTransformBlocks_.resize(numBlocksVertical);
	for (uint32_t i = 0; i < numBlocksVertical; ++i) {
		WorldTransformBlocks_[i].resize(numBlocksHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < numBlocksVertical; ++i) {

		for (uint32_t j = 0; j < numBlocksHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				WorldTransformBlocks_[i][j] = worldTransform;
				WorldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());
	skydome_->Draw();

	for (std::vector<WorldTransform*>& WorldTransformBlockLine : WorldTransformBlocks_) {
		for (WorldTransform* WorldTransformBlock : WorldTransformBlockLine) {
			if (!WorldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*WorldTransformBlock, camera_);
		}
	}

	// model_->Draw(worldTransform_,camera_,textureHandle_);

	if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn) {
		player_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	fade_->Draw();
}
