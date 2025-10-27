#include "GameScene.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include "Mino.h"
#include <map>

using namespace KamataEngine;

// デストラクタ
GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
	delete modelBlock_;
	delete modelSkydome_;
	delete skydome_;
	delete mapChipField_;
	delete fade_;
	delete mino_;

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
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	debugCamera_ = new DebugCamera(1280, 720);

	// ワールド
	worldTransform_.Initialize();

	// カメラ
	camera_.Initialize();

	mapChipField_ = new MapChipField();
	mapChipField_->LodeMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);
	
	phase_ = Phase::kPlay;

	fade_ = new Fade();
	fade_->Initialize();                     // フェードの初期化
	fade_->Start(Fade::State::FadeIn, 1.0f); // フェードインを開始

	mino_ = new Mino();
	mino_->SetMapChipField(mapChipField_);
	mino_->GenerateMino(modelBlock_, &camera_);
}

void GameScene::Update() {

	fade_->Update();

	switch (phase_) {
	case Phase::kPlay:
		
		break;
	case Phase::kDeath:

		
		break;
	case Phase::kFadeIn:
		fade_->Update();
		break;
	case Phase::kFadeOut:
		fade_->Update();
		break;
	}

	debugCamera_->Update();
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
		
		camera_.TransferMatrix();
	}

	mino_->Update();

	GenerateBlocks();

	for (std::vector<WorldTransform*>& WorldTransformBlockLine : WorldTransformBlocks_) {
		for (WorldTransform* WorldTransformBlock : WorldTransformBlockLine) {
			if (!WorldTransformBlock) {
				continue;
			}
			WorldTransformBlock->matWorld_ = MakeAffineMatrix(WorldTransformBlock->scale_, WorldTransformBlock->rotation_, WorldTransformBlock->translation_);
			WorldTransformBlock->TransferMatrix();
		}
	}

	CheckAllCollision();

	ChangePhase();
}

void GameScene::CheckAllCollision() {
	
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		
		break;
	case Phase::kDeath:
		
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

	// ブロックの生成
	for (uint32_t i = 0; i < numBlocksVertical; ++i) {

		for (uint32_t j = 0; j < numBlocksHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) != MapChipType::kBlank) {
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

	// ブロックの描画
	for (std::vector<WorldTransform*>& WorldTransformBlockLine : WorldTransformBlocks_) {
		for (WorldTransform* WorldTransformBlock : WorldTransformBlockLine) {
			if (!WorldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*WorldTransformBlock, camera_);
		}
	}

	mino_->Draw();

	// model_->Draw(worldTransform_,camera_,textureHandle_);

	Model::PostDraw();

	fade_->Draw();
}
