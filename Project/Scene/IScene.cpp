#include "IScene.h"
#include "Object3D.h"
#include "../../externals/nlohmann/json.hpp"

int IScene::sceneNum;

IScene::~IScene() {

}

void IScene::Finalize() {
	for (Object3D* object : levelObjects_) {
		delete object;
	}
	levelObjects_.clear();
	enemyPoints_.clear();
	models_.clear();
}

void IScene::LoadJSONFile(const std::string fileName) {
	// ファイルのフルパス
	const std::string fullPath = "Engine/resources/Level/" + fileName;
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
		std::string name = object["name"].get<std::string>();
		// 敵の出現場所かの判別をする
		std::string checkSpawnPoint = "enemy_point";

		if (type.compare("CURVE") == 0) {
			// 要素追加
			levelData->railCameraControlPoint_.emplace_back(LevelData::CurveData{});
			// 追加した要素の参照を取得
			LevelData::CurveData& railCameraData = levelData->railCameraControlPoint_.back();

			// トランスフォームのパラメータ読み込み
			assert(object.contains("curve_data"));
			nlohmann::json& curve_data = object["curve_data"];
			assert(curve_data.is_array());

			// 各制御点を走査
			for (const auto& pointData : curve_data) {
				assert(pointData.contains("point"));
				const auto& point = pointData["point"];
				assert(point.is_array());
				assert(point.size() == 3);

				// ワールド座標に変換して追加
				railCameraData.points.emplace_back(Vector3{
					(float)point[0],
					(float)point[2],
					(float)point[1]
					});
			}
			controlPoints_ = railCameraData.points;
		}
		// MESH
		if (type.compare("MESH") == 0) {
			if(name.substr(0,11) == checkSpawnPoint.substr(0,11)) {
				// 要素追加
				levelData->enemyPoints_.emplace_back(Vector3{});
				// 追加した要素の参照を取得
				Vector3& enemyPoint = levelData->enemyPoints_.back();
				nlohmann::json& pos = object["spawn_data"][0];
				enemyPoint = Vector3{
					(float)pos["point"][0],
					(float)pos["point"][2],
					(float)pos["point"][1]
					};

				enemyPoints_.push_back(enemyPoint);
			}
			else {
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
		//newObject->StartAnim(0);
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
