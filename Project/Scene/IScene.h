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

/// <summary>
/// シーンの基底クラス
/// </summary>
class IScene
{
public:
	virtual ~IScene();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	int GetSceneNum() { return sceneNum; }
protected:
	static int sceneNum;
};
