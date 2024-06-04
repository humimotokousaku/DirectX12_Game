#pragma once
#include "Object3D.h"
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
	virtual void Finalize() = 0;

	/// <summary>
	/// JSONファイルの読み込み
	/// </summary>
	void LoadJSONFile(const std::string fileName);

	int GetSceneNum() { return sceneNum; }
protected:
	static int sceneNum;
	std::vector<Object3D*> levelObjects_;
};
