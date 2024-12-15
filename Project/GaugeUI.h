#pragma once
#include "Camera.h"
#include "DirectXCommon.h"
#include "MathStructs.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include "WinApp.h"
#include "WorldTransform.h"
#include "Sprite.h"
#include <d3d12.h>
#include <wrl.h>

class GaugeUI {
public:
	GaugeUI() = default;
	~GaugeUI() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	//void update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:// エンジン機能
	TextureManager* textureManager_;
	PipelineManager* psoManager_;

private:
	// カメラ
	std::unique_ptr<Camera> camera_;

	std::unique_ptr<Sprite> sprite_;

	// 描画をするか
	bool isActive_ = true;
};