#pragma once
#include "IPostEffect.h"
#include "Sprite.h"
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

public:
	std::vector<IPostEffect*> postEffect_;
	std::list<Sprite*> sprites_;
	std::unique_ptr<Camera> camera_;
	std::vector<uint32_t> renderTexture_;
};