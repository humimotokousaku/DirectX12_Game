#pragma once
#include "ModelManager.h"
#include "Object3D.h"
#include "../../externals/nlohmann/json.hpp"

/// <summary>
/// ブレンダーで作成したjson情報を読み込むクラス
/// </summary>
class LevelManager {
public:
	// シングルトン
	static LevelManager* GetInstance();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// JSONファイルの読み込み
	/// </summary>
	/// <param name="fileName">Blenderで出力したjsonファイルの名前</param>
	/// <param name="camera">カメラのアドレス</param>
	void LoadJSONFile(const std::string fileName, Camera* camera);

	/// <summary>
	/// jsonのデータを読み取ってVector3型に変換
	/// </summary>
	/// <param name="v">x,y,zの情報,もしくはそれに準ずるもの</param>
	/// <returns>Vector3</returns>
	Vector3 ConvertVector3(nlohmann::json& v) {
		Vector3 result{};
		result.x = (float)v[0];
		result.y = (float)v[2];
		result.z = (float)v[1];
		return result;
	}

#pragma region Setter
	/// <summary>
	/// 使用するカメラの設定
	/// </summary>
	/// <param name="camera"></param>
	void SetCamera(Camera* camera) { 
		camera_ = camera;
		for (Object3D* object : levelObjects_) {
			object->SetCamera(camera_);
		}
	}
#pragma endregion

#pragma region Getter
	/// <summary>
	/// レールカメラの制御点を取得
	/// </summary>
	/// <param name="controlPoints">制御点</param>
	/// <returns></returns>
	std::vector<Vector3> GetRailCameraControlPoints() { return controlPoints_; }
	/// <summary>
	/// 敵の発生情報を取得
	/// </summary>
	/// <param name="enemyPoints"></param>
	/// <returns></returns>
	std::vector<LevelData::EnemyPoint> GetEnemyPoints() { return enemyPoints_; }
#pragma endregion

private:
	// カメラのアドレス
	Camera* camera_;

	// ブレンダーで読み込むオブジェクト
	std::vector<Object3D*> levelObjects_;
	// レールカメラの移動ルートの制御点
	std::vector<Vector3> controlPoints_;
	// 敵の発生情報
	std::vector<LevelData::EnemyPoint> enemyPoints_;
};