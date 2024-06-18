#include "IScene.h"
#include "Object3D.h"
#include "../../externals/nlohmann/json.hpp"

int IScene::sceneNum;

IScene::~IScene() {
	for (Object3D* object : levelObjects_) {
		delete object;
	}
	levelObjects_.clear();
	models_.clear();
}

void IScene::LoadJSONFile(const std::string fileName) {
	// ファイルのフルパス
	const std::string fullPath = "engine/resources/level/" + fileName;
	// ファイルストリーム
	std::ifstream file;
	// ファイルを開く
	file.open(fullPath);
	// ファイルオープン失敗を確認
	if (file.fail()) {
		assert(0);
	}

	std::vector<Object3D*> objects;

	// JSON文字列から回答したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルか確認
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかを確認
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンス生成
	std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// 要素追加
			levelData->objects_.emplace_back(LevelData::ObjectData{});
			// 追加した要素の参照を取得
			LevelData::ObjectData& objectData = levelData->objects_.back();

			if (object.contains("file_name")) {
				// ファイル名
				objectData.fileName = object["file_name"];
			}
			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// 平行移動
			objectData.translate.x = (float)transform["translation"][0];
			objectData.translate.y = (float)transform["translation"][2];
			objectData.translate.z = (float)transform["translation"][1];
			// 回転角
			Vector3 rotate = { Degree2Radian(-(float)transform["rotation"][0]) ,Degree2Radian(-(float)transform["rotation"][2]),Degree2Radian(-(float)transform["rotation"][1]) };
			objectData.rotate = rotate;
			// スケーリング
			objectData.scale.x = (float)transform["scaling"][0];
			objectData.scale.y = (float)transform["scaling"][2];
			objectData.scale.z = (float)transform["scaling"][1];
		}

		if (object.contains("children")) {

		}
	}

	for (auto& objectData : levelData->objects_) {	
		Model* model;
		// モデルの読み込み
		ModelManager::GetInstance()->LoadModel("level", objectData.fileName);
		// ファイル名から登録済みモデルを検索
		model = (ModelManager::GetInstance()->FindModel("level", objectData.fileName));

		// モデルを指定して3Dオブジェクト生成
		Object3D* newObject = new Object3D();
		newObject->Initialize();
		newObject->SetModel(model);
		// 座標
		newObject->worldTransform.translate = objectData.translate;
		// 回転
		newObject->worldTransform.rotate = objectData.rotate;
		// 座標
		newObject->worldTransform.scale = objectData.scale;

		objects.push_back(newObject);
	}

	levelObjects_ = objects;
}
