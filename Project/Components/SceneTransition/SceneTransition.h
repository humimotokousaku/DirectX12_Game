#pragma once
#include "Animation.h"
#include "Sprite.h"

class SceneTransition {
public:
	void Initialize();
	void Update();

	// 遷移開始の演出
	void BeginEffect();
	// 遷移終了の演出
	void EndEffect();
private:
	Animation beginAnim_;
	Animation endAnim_;
	std::array<Sprite, 3> sprite_;
	std::array<Vector3*, 3> spritePos_;
};