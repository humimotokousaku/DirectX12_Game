#include "LevelManager.h"
#include "../../externals/nlohmann/json.hpp"

LevelManager* LevelManager::GetInstance() {
	static LevelManager instance;
	return &instance;
}

void LevelManager::Draw() {
	for (Object3D* object : levelObjects_) {
		object->Draw();
	}
}

void LevelManager::LoadJSONFile(const std::string fileName, Camera* camera) {
	// カメラのアドレスを代入
	camera_ = camera;

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
				const auto& point = pointData["point"];

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
			if (name.substr(0, 11) == checkSpawnPoint.substr(0, 11)) {
				// 要素追加
				levelData->enemyPoints_.emplace_back(LevelData::EnemyPoint{});
				// 追加した要素の参照を取得
				LevelData::EnemyPoint& enemyPoint = levelData->enemyPoints_.back();
				nlohmann::json& spawnData = object["spawn_data"][0];

				// 出現場所
				enemyPoint.point.x = (float)spawnData["point"][0];
				enemyPoint.point.y = (float)spawnData["point"][2];
				enemyPoint.point.z = (float)spawnData["point"][1];
				// 初期の向き
				Vector3 rotate = { Degree2Radian(-(float)spawnData["rotation"][0]) ,Degree2Radian(-(float)spawnData["rotation"][2]),Degree2Radian(-(float)spawnData["rotation"][1]) };
				enemyPoint.rotate = rotate;

				// 出現タイミング
				enemyPoint.percentage = (float)spawnData["percentage"];

				// 種類
				enemyPoint.type = spawnData["enemyTypes"].get<std::string>();

				// 移動速度(方向ベクトルに対して扱うものとする)
				enemyPoint.velocity.x = (float)spawnData["velocity"][0];
				enemyPoint.velocity.y = (float)spawnData["velocity"][1];
				enemyPoint.velocity.z = (float)spawnData["velocity"][2];

				// 
				if (object.contains("children")) {
					nlohmann::json& children = object["children"][0];
					// 追加した要素の参照を取得
					LevelData::CurveData& curveData = enemyPoint.controlPoints;

					// 各制御点を走査
					for (const auto& pointData : children["curve_data"]) {
						const auto& point = pointData["point"];

						// ワールド座標に変換して追加
						curveData.points.emplace_back(Vector3{
							(float)point[0],
							(float)point[2],
							(float)point[1]
							});
					}
					enemyPoint.controlPoints = curveData;
				}

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
				// 背景用のオブジェクトか
				nlohmann::json& isSkydome = object["skydome"];
				objectData.isSkydome = (int)isSkydome;

				// 当たり判定が設定されているか
				if (object.contains("collider")) {
					nlohmann::json& collider = object["collider"];
					// OBBの場合
					if (collider["type"] == "OBB") {
						// 使用するコライダーの種類
						objectData.colliderType = collider["type"];
						// 当たり判定の大きさ
						objectData.colliderSize.x = collider["size"][0];
						objectData.colliderSize.y = collider["size"][2];
						objectData.colliderSize.z = collider["size"][1];
					}
				}
			}
		}
	}

	std::vector<Object3D*> objects;
	for (auto& objectData : levelData->objects_) {
		Model* model;
		// モデルの読み込み
		ModelManager::GetInstance()->LoadModel("level", objectData.fileName);
		// ファイル名から登録済みモデルを検索
		model = (ModelManager::GetInstance()->FindModel("level", objectData.fileName));

		// モデルを指定して3Dオブジェクト生成
		Object3D* newObject = new Object3D();
		newObject->Initialize();

		// 見た目のモデルを設定
		newObject->SetModel(model);

		// カメラの設定
		newObject->SetCamera(camera_);

		// 座標
		newObject->worldTransform.translate = objectData.translate;
		// 回転
		newObject->worldTransform.rotate = objectData.rotate;
		// 座標
		newObject->worldTransform.scale = objectData.scale;

		// ライティングを行う
		newObject->SetIsLighting(true);
		// 背景用のオブジェクトなら光を適用しない
		if (objectData.isSkydome) {
			newObject->SetIsLighting(false);
		}

		// 当たり判定(今回は障害物限定)
		if (objectData.colliderType == "OBB") {
			// 当たり判定の設定
			newObject->collider->worldTransform.parent_ = &newObject->worldTransform;
			newObject->collider->SetCollisionPrimitive(kCollisionOBB);
			newObject->collider->SetCollisionAttribute(kCollisionAttributeObstacles);
			newObject->collider->SetCollisionMask(~kCollisionAttributeObstacles);
			newObject->collider->SetIsActive(true);
			// 当たり判定の大きさを代入
			newObject->collider->SetOBBLength(objectData.colliderSize);
		}

		objects.push_back(newObject);
	}

	levelObjects_ = objects;
}

void LevelManager::Finalize() {
	for (Object3D* object : levelObjects_) {
		delete object;
	}
	levelObjects_.clear();
	enemyPoints_.clear();
}
