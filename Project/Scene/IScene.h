#pragma once
#include "Object3D.h"
#include "PostEffectManager.h"

#include <string>
#include <vector>

enum Scene {
	TITLE_SCENE,
	GAME_SCENE,
	GAMECLEAR_SCENE,
	GAMEOVER_SCENE
};

class IScene
{
public:
	virtual ~IScene();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize();

	/// <summary>
	/// JSONファイルの読み込み
	/// </summary>
	void LoadJSONFile(const std::string fileName);

	void AddModel(Model* model) { models_.push_back(model); }

	int GetSceneNum() { return sceneNum; }
protected:
	static int sceneNum;
	// ブレンダーで読み込むオブジェクト
	std::vector<Object3D*> levelObjects_;
	// レールカメラの移動ルートの制御点
	std::vector<Vector3> controlPoints_;
	// 敵の出現場所
	std::vector<LevelData::EnemyPoint> enemyPoints_;

	// 登録用のモデル
	std::vector<Model*> models_;
};
