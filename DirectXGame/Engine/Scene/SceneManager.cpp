#include "SceneManager.h"

SceneManager::SceneManager() : currentScene_(SceneType::kUnknown), gameScene_(nullptr), titleScene_(nullptr){}

SceneManager::~SceneManager() {
	delete gameScene_;
	delete titleScene_;
	delete gameOverScene_;
}

void SceneManager::Initialize() {
	currentScene_ = SceneType::kTitle;
	titleScene_ = new TitleScene();
	titleScene_->Initialize();
	gameScene_ = nullptr;
}

void SceneManager::ChangeScene() {
	switch (currentScene_) {
	case SceneType::kTitle:
		if (titleScene_ && titleScene_->IsFinished()) {
			currentScene_ = SceneType::kGame;
			delete titleScene_;
			titleScene_ = nullptr;
			gameScene_ = new GameScene();
			gameScene_->Initialize();
		}
		break;
	case SceneType::kGame:
		if (gameScene_ && gameScene_->IsFinished()) {
			currentScene_ = SceneType::kGameOver;
			delete gameScene_;
			gameScene_ = nullptr;
			gameOverScene_ = new GameOverScene();
			gameOverScene_->Initialize();
		}
		break;
	case SceneType::kGameOver:
		if (gameOverScene_ && gameOverScene_->IsFinished()) {
			currentScene_ = SceneType::kTitle;
			delete gameOverScene_;
			gameOverScene_ = nullptr;
			titleScene_ = new TitleScene();
			titleScene_->Initialize();
		}
		break;
	}
}

void SceneManager::ChangeScene() {
	switch (currentScene_) {
	case SceneType::kTitle:
		if (titleScene_ && titleScene_->IsFinished()) {
			currentScene_ = SceneType::kGame;
			delete titleScene_;
			titleScene_ = nullptr;
			gameScene_ = new GameScene();
			gameScene_->Initialize();
		}
		break;
	case SceneType::kGame:
		if (gameScene_ && gameScene_->IsFinished()) {
			currentScene_ = SceneType::kGameOver;
			delete gameScene_;
			gameScene_ = nullptr;
			gameOverScene_ = new GameOverScene();
			gameOverScene_->Initialize();
		}
		break;
	case SceneType::kGameOver:
		if (gameOverScene_ && gameOverScene_->IsFinished()) {
			currentScene_ = SceneType::kTitle;
			delete gameOverScene_;
			gameOverScene_ = nullptr;
			titleScene_ = new TitleScene();
			titleScene_->Initialize();
		}
		break;
	}
}


void SceneManager::Update() {
	switch (currentScene_) {
	case SceneType::kTitle:
		if (titleScene_)
			titleScene_->Update();
		break;
	case SceneType::kGame:
		if (gameScene_)
			gameScene_->Update();
		break;
	case SceneType::kGameOver:
		if (gameOverScene_)
			gameOverScene_->Update();
		break;
	}
}

void SceneManager::Draw() {
	switch (currentScene_) {
	case SceneType::kTitle:
		if (titleScene_)
			titleScene_->Draw();
		break;
	case SceneType::kGame:
		if (gameScene_)
			gameScene_->Draw();
		break;
	case SceneType::kGameOver:
		if (gameOverScene_)
			gameOverScene_->Draw();
		break;
	}
}