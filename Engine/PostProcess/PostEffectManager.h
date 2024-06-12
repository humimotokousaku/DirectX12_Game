#pragma once
#include "PostEffect.h"
#include "RadialBlur.h"
#include "Bloom.h"
#include "Gauss.h"
#include "Dissolve.h"
#include "PostEffectPSO.h"
#include "Object3D.h"

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
	std::unique_ptr<Object3D> object_;
	std::unique_ptr<Camera> camera_;
	std::vector<uint32_t> renderTexture_;
};