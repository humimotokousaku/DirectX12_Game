#include "LevelManager.h"
#include "GameObjectManager.h"

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
		std::string objectName = object["name"].get<std::string>();
		// 敵の出現場所かの判別をする
		std::string checkSpawnPoint = "enemy_point";

		// レールカメラ
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
		// オブジェクト
		if (type.compare("MESH") == 0) {
			// 敵のスポーン情報
			if (objectName.substr(0, 11) == checkSpawnPoint.substr(0, 11)) {
				// 要素追加
				levelData->enemyPoints_.emplace_back(LevelData::EnemyPoint{});
				// 追加した要素の参照を取得
				LevelData::EnemyPoint& enemyPoint = levelData->enemyPoints_.back();
				nlohmann::json& spawnData = object["spawn_data"][0];

				// 出現場所
				enemyPoint.point = ConvertVector3(spawnData["point"]);
				// 回転角(DegreeからRadianに変換)
				Vector3 rotate = { Degree2Radian(-(float)spawnData["rotation"][0]) ,Degree2Radian(-(float)spawnData["rotation"][2]),Degree2Radian(-(float)spawnData["rotation"][1]) };
				enemyPoint.rotate = rotate;

				// 出現タイミング
				enemyPoint.percentage = (float)spawnData["percentage"];

				// 種類
				enemyPoint.type = spawnData["enemyTypes"].get<std::string>();

				// 移動速度(方向ベクトルとして扱う)
				enemyPoint.velocity = ConvertVector3(spawnData["velocity"]);

				// 敵の移動ルート
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
				continue;
			}

			/// 障害物の情報

			// 要素追加
			levelData->objects_.emplace_back(LevelData::ObjectData{});
			// 追加した要素の参照を取得
			LevelData::ObjectData& objectData = levelData->objects_.back();
			
			// モデルのファイル名
			if (object.contains("file_name")) {	
				objectData.fileName = object["file_name"];
			}

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// 平行移動
			objectData.translate = ConvertVector3(transform["translation"]);
			// 回転角
			Vector3 rotate = { Degree2Radian(-(float)transform["rotation"][0]) ,Degree2Radian(-(float)transform["rotation"][2]),Degree2Radian(-(float)transform["rotation"][1]) };
			objectData.rotate = rotate;
			// スケーリング
			objectData.scale = ConvertVector3(transform["scaling"]);

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
					objectData.colliderSize = ConvertVector3(collider["size"]);
				}
			}
		}
	}

	// オブジェクト生成
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
		// モデルを設定
		newObject->SetModel(model);
		// カメラの設定
		newObject->SetCamera(camera_);

		// 座標
		newObject->worldTransform.translate = objectData.translate;
		// 回転
		newObject->worldTransform.rotate = objectData.rotate;
		// 座標
		newObject->worldTransform.scale = objectData.scale;

		newObject->worldTransform.UpdateMatrix();

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
			newObject->collider->SetDamage(30.0f);
			newObject->collider->SetIsActive(true);
			// 当たり判定の大きさを代入
			newObject->collider->SetOBBLength(objectData.colliderSize);
		}

		objects.push_back(newObject);

		// ゲームオブジェクトマネージャーに追加
		GameObjectManager::GetInstance()->AddGameObject(newObject);
	}

	levelObjects_ = objects;
}

void LevelManager::Finalize() {
	for (Object3D* object : levelObjects_) {
		delete object;
	}
	levelObjects_.clear();
	controlPoints_.clear();
	enemyPoints_.clear();
}
