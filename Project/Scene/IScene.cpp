#include "IScene.h"
#include "Audio.h"
#include "Object3D.h"

int IScene::sceneNum;

IScene::~IScene() {
	// 音を消す
	Audio::GetInstance()->SoundUnload();
	// ゲームパッドの振動を消す
	Input::GetInstance()->GamePadVibration(0, 0, 0);
}