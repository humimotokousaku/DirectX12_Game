#include "IScene.h"
#include "Object3D.h"
#include "../../externals/nlohmann/json.hpp"

int IScene::sceneNum;

IScene::~IScene() {
	/*for (Object3D* object : levelObjects_) {
		delete object;
	}*/
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
		// ファイル名から登録済みモデルを検索
		Model* model = new Model();
		decltype(ModelManager::GetInstance()->models_)::iterator it = ModelManager::GetInstance()->models_.find(objectData.fileName);
		if (it != ModelManager::GetInstance()->models_.end()) {		
			model = it->second.get();
		}
		model->Initialize("level/" + objectData.fileName);
		// モデルを指定して3Dオブジェクト生成
		std::unique_ptr<Object3D> newObject = std::make_unique<Object3D>();
		newObject->Initialize();
		newObject->SetModel(model);
		// 座標
		newObject->worldTransform.transform.translate = objectData.translate;
		// 回転
		newObject->worldTransform.transform.rotate = objectData.rotate;
		// 座標
		newObject->worldTransform.transform.scale = objectData.scale;

		objects.push_back(newObject.get());

		// モデルをmapコンテナに格納
		ModelManager::GetInstance()->models_.insert(std::make_pair("engine/resources/level" + fileName, std::move(model)));
	}

	levelObjects_ = objects;
}
