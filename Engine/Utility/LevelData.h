#pragma once
#include "ModelStructs.h"
#include <vector>

class LevelData {
public:
	// 障害物
	struct ObjectData {
		Vector3 scale;			// 大きさ
		Vector3 rotate;			// オイラー角
		Vector3 translate;		// 座標
		std::string fileName;	// モデルのファイル名
	};
	// レールカメラの移動ルート
	struct CurveData {
		std::string type;				// カーブの種類
		std::vector<Vector3> points;	// 制御点
	};
	// 敵の出現場所
	struct EnemyPoint {
		Vector3 point;		// 座標
		Vector3 rotate;		// 向き
		float percentage;	// 出現条件:カーブの割合
	};
	std::vector<Transform> transform_;
	std::vector<std::string> type_;
	std::vector<std::string> name_;

	std::vector<EnemyPoint> enemyPoints_;

	std::vector<ObjectData> objects_;

	std::vector<CurveData> railCameraControlPoint_;
};