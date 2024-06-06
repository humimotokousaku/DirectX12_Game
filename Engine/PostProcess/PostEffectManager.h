#pragma once
#include "PostEffect.h"
#include "RadialBlur.h"
#include "Bloom.h"
#include "Gauss.h"
#include "Dissolve.h"
#include "PostEffectPSO.h"

class PostEffectManager {
public:
	PostEffectManager() = default;
	~PostEffectManager();
	void Initialize();
	void PreDraw();
	void PostDraw();
	void Draw();

public:
	std::vector<IPostEffect*> postEffect_;
};