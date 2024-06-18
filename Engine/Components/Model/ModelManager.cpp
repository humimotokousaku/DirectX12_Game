#include "ModelManager.h"
#include "WinApp.h"
#include "Object3D.h"

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;

	return &instance;
}

void ModelManager::Initialize() {
	texManager_ = TextureManager::GetInstance();
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filePath) {
	// 読み込み済みモデルを検索
	if (models_.contains("Engine/resources/" + directoryPath + "/" + filePath)) {
		// 読み込み済みなら
		return;
	}

	// モデル生成とファイル読み込み
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(directoryPath, filePath);

	// モデルをmapコンテナに格納
	models_.insert(std::make_pair("Engine/resources/" + directoryPath + "/" + filePath, std::move(model)));
}

void ModelManager::LoadModel(const std::string& fileFullPath) {
	// 読み込み済みモデルを検索
	if (models_.contains(fileFullPath)) {
		// 読み込み済みなら
		return;
	}

	// モデル生成とファイル読み込み
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(fileFullPath);

	// モデルをmapコンテナに格納
	models_.insert(std::make_pair(fileFullPath, std::move(model)));
}

void ModelManager::Finalize() {
	//models_.clear();
	//delete instance;
	//instance = nullptr;
	//
}

Model* ModelManager::FindModel(const std::string& directoryPath, const std::string& filePath) {
	// 読み込み済みモデルを検索
	if (models_.contains("Engine/resources/" + directoryPath + "/" + filePath)) {
		// 読み込み済みモデルを返す
		return models_.at("Engine/resources/" + directoryPath + "/" + filePath).get();
	}

	// 該当ファイルなし
	WinApp::Log("Not ModelFile\n");
	return nullptr;
}
Model* ModelManager::FindModel(const std::string& fileFullPath) {
	// 読み込み済みモデルを検索
	if (models_.contains(fileFullPath)) {
		// 読み込み済みモデルを返す
		return models_.at(fileFullPath).get();
	}

	// 該当ファイルなし
	WinApp::Log("Not ModelFile\n");
	return nullptr;
}

Model* ModelManager::SetModel(const std::string& directoryPath, const std::string& filePath) {
	return ModelManager::GetInstance()->FindModel(directoryPath, filePath);
}