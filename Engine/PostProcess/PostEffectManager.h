#pragma once
#include "IPostEffect.h"
#include "PostEffect.h"
#include "RadialBlur.h"
#include "Bloom.h"
#include "Gauss.h"
#include "Dissolve.h"
#include "Outline.h"
#include "Vigneting.h"
#include "Sprite.h"
#include "PostEffectStructs.h"
#include <list>

class PostEffectManager {
public:
	static PostEffectManager* GetInstance();
	PostEffectManager() = default;
	~PostEffectManager();
	void Initialize();
	void PreDraw();
	void PostDraw();
	void Draw();

	/// <summary>
	/// ポストエフェクトを使用しないスプライトを追加
	/// </summary>
	/// <param name="sprite">ポストエフェクトを使用しないスプライト</param>
	void AddSpriteList(Sprite* sprite) { sprites_.push_back(sprite); }

	/// <summary>
	/// ポストエフェクトを使用しないスプライトリストをクリア
	/// </summary>
	void ClearSpriteList() { sprites_.clear(); }

	/// <summary>
	/// ポストエフェクトをすべてオフにする
	/// </summary>
	void ResetPostEffect() {
		radialBlurData_.isActive = false;
		gaussData_.isActive = false;
		bloomData_.isActive = false;
		vignetingData_.isActive = false;
		dissolveData_.isActive = false;
	}

public:
	RadialBlurData radialBlurData_;
	GaussData gaussData_;
	BloomData bloomData_;
	VignetingData vignetingData_;
	DissolveData dissolveData_;

private:
	std::vector<IPostEffect*> postEffect_;
	std::list<Sprite*> sprites_;
	std::vector<uint32_t> renderTexture_;

	PostEffect* normal_;
	// RadialBlur
	RadialBlur* radialBlur_;
	// Gauss
	Gauss* gauss_;
	// Bloom
	Bloom* bloom_;
	// Vigneting
	Vigneting* vigneting_;
	// Dissolve
	Dissolve* dissolve_;
};