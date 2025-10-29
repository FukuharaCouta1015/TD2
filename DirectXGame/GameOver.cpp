#include "MyMath.h"
#include "GameOver.h"
#include <numbers>

using namespace KamataEngine;

// デストラクタ
GameOverScene::~GameOverScene() {
	delete model_;
	delete fade_;
	delete gameOver_;
}

void GameOverScene::Initialize() {
	model_ = Model::CreateFromOBJ("titleFont");
	gameOverHandle_ = TextureManager::Load("./resources/GameOver.png");
	gameOver_ = Sprite::Create(gameOverHandle_, {0, 0});

	camera_.Initialize();

	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.translation_ = {0, 0, 0};

	fade_ = new Fade();
	fade_->Initialize();                     // フェードの初期化
	fade_->Start(Fade::State::FadeIn, 1.0f); // フェードインを開始
}

void GameOverScene::Update() {

	switch (phase_) {
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE) || Input::GetInstance()->IsTriggerMouse(0)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::State::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain; // フェードインが完了したらメインフェーズに移行
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true; // フェードアウトが完了したらタイトルシーンを終了
		}
	}

	// アフィン変換行列の生成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

	// 回転角度を更新
	rotate += 0.1f;                                                              // 回転速度を調整
	worldTransform_.rotation_.y = -sin(rotate);                                  // Y軸を中心に回転
}

void GameOverScene::Draw() {
	// DirectXCommonのインスタンスを取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 描画開始
	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, camera_);

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	if (gameOver_) {
		gameOver_->Draw();
	}

	Sprite::PostDraw();

	fade_->Draw(); // フェードの描画
}
