#pragma once
#include "ModelStructs.h"
#include <vector>

/// <summary>
/// ブレンダーで読み取るオブジェクトと数値をまとめたクラス
/// </summary>
class LevelData {
public:
	// 障害物
	struct ObjectData {
		Vector3 scale;			// 大きさ
		Vector3 rotate;			// オイラー角
		Vector3 translate;		// 座標
		std::string fileName;	// モデルのファイル名
		bool isSkydome;			// 背景用のオブジェクトか
		std::string colliderType;// 当たり判定の種類
		Vector3 colliderSize;	// 当たり判定の大きさ
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
		CurveData controlPoints; // 移動ルートの制御点
		float percentage;	// 出現条件:カーブの割合
		std::string type;	// 種類
		Vector3 velocity;	// 初速度(移動ベクトル)
		bool isActive = false;// 出現済みか
	};
	std::vector<Transform> transform_;
	std::vector<std::string> type_;
	std::vector<std::string> name_;

	// 敵の発生情報
	std::vector<EnemyPoint> enemyPoints_;

	// 読みこんだオブジェクト
	std::vector<ObjectData> objects_;

	// レールカメラの制御点
	std::vector<CurveData> railCameraControlPoint_;
};