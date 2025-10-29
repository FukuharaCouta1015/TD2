#include "GameScene.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "MapChipField.h"
#include "Mino.h"
#include "MyMath.h"
#include "Player.h"
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
	delete spriteRight_;
	delete spriteLeft_;

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
	modelMino_ = Model::CreateFromOBJ("block");
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
	mino_->GenerateMino(modelMino_, &camera_);

	// スプライト（左上アンカー扱いで位置を決める）
	spriteRightPos_ = {300.0f, 300.0f, 0.0f}; // 右移動ボタン（左上）
	// 左隣は右の左にサイズ分+余白で配置
	const float spacing = 10.0f;
	spriteLeftPos_ = {spriteRightPos_.x - spriteSize_.x - spacing, spriteRightPos_.y, 0.0f};

	spriteRight_ = Sprite::Create(textureHandleRight_, {spriteRightPos_.x, spriteRightPos_.y});
	spriteRight_->SetSize({spriteSize_.x, spriteSize_.y});

	spriteLeft_ = Sprite::Create(textureHandleLeft_, {spriteLeftPos_.x, spriteLeftPos_.y});
	spriteLeft_->SetSize({spriteSize_.x, spriteSize_.y});
}

void GameScene::Update() {
	auto mousePos = Input::GetInstance()->GetMousePosition();
	ImGui::Begin("Window");
	ImGui::Text("mousePos %d %d", static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));
	ImGui::End();

	// マウス左クリック（トリガー）で現在の座標を取得してログ出力
	if (Input::GetInstance()->IsTriggerMouse(0)) { // 0 = 左ボタン
		auto pos = Input::GetInstance()->GetMousePosition();
	}

	// --- スプライト左右両方の当たり判定を関数化して処理 ---
	// 左移動ボタン（左隣） -> dx = -1
	ProcessSpriteClickMove(spriteLeftPos_, spriteSize_, -1);
	// 右移動ボタン（元の位置） -> dx = +1
	ProcessSpriteClickMove(spriteRightPos_, spriteSize_, +1);

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
		camera_.translation_ = {0.0f, 10.0f, -50.0f};
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

bool GameScene::ProcessSpriteClickMove(const KamataEngine::Vector3& spriteLeftTop, const KamataEngine::Vector3& spriteSize, int dx) {
	// マウス座標（クライアントピクセル、左上原点）を取得
	auto mousePos = Input::GetInstance()->GetMousePosition();

	// スプライトは左上アンカー前提で当たり判定
	float left = spriteLeftTop.x;
	float right = spriteLeftTop.x + spriteSize.x;
	float top = spriteLeftTop.y;
	float bottom = spriteLeftTop.y + spriteSize.y;

	// クリックがトリガーかつマウスがスプライト内ならミノに移動要求を送る
	if (Input::GetInstance()->IsTriggerMouse(0) && mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom) {
		if (mino_) {
			mino_->RequestMove(dx);
			return true;
		}
	}
	return false;
}

void GameScene::CheckAllCollision() {}

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

	Sprite::PreDraw(dxCommon->GetCommandList());

	if (spriteRight_) {
		spriteRight_->Draw();
	}
	if (spriteLeft_) {
		spriteLeft_->Draw();
	}

	Sprite::PostDraw();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());
	skydome_->Draw();

	// ブロックの描画（MapChipType に応じて適切なモデル/テクスチャだけ描画する）
	for (size_t i = 0; i < WorldTransformBlocks_.size(); ++i) {
		for (size_t j = 0; j < WorldTransformBlocks_[i].size(); ++j) {
			WorldTransform* WorldTransformBlock = WorldTransformBlocks_[i][j];
			if (!WorldTransformBlock) {
				continue;
			}
			// マップのインデックスは j が横、i が縦（GenerateBlocks と同じ順）
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(static_cast<uint32_t>(j), static_cast<uint32_t>(i));
			if (type == MapChipType::kBlock) {
				modelBlock_->Draw(*WorldTransformBlock, camera_);
			} else if (type == MapChipType::kMino) {
				modelMino_->Draw(*WorldTransformBlock, camera_, textureHandleMino_);
			}
		}
	}

	mino_->Draw();

	// model_->Draw(worldTransform_,camera_,textureHandle_);

	Model::PostDraw();

	fade_->Draw();
}