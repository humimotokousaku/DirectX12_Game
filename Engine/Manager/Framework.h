#pragma once
// Scene
#include "IScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameClear.h"
#include "GameOver.h"

// PSO
#include "PipelineManager.h"

// Base
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

// components
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Input.h"
#include "Audio.h"
#include "ModelManager.h"
#include "PostEffect.h"
#include "PostEffectManager.h"

class Framework {
public:
	///
	/// Default Method
	/// 

	// コンストラクタ
	Framework();
	// デストラクタ
	virtual ~Framework() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();	
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 解放処理
	/// </summary>
	virtual void Finalize();
	/// <summary>
	/// 描画前の処理
	/// </summary>
	virtual void BeginFrame();
	/// <summary>
	/// 描画後の処理
	/// </summary>
	virtual void EndFrame();

	/// 
	/// User Method
	/// 

	/// <summary>
	/// ループ処理
	/// </summary>
	void Run();

protected:
	// base
	WinApp* winApp_;
	DirectXCommon* directXCommon_;
	SrvManager* srvManager_;
	// PSO
	PipelineManager* pipelineManager_;

	PostEffectManager* postEffectManager_;
	PostEffect* postEffect_;

	// components
	DirectionalLight* directionalLight_;
	PointLight* pointLight_;
	SpotLight* spotLight_;
	//ModelManager* modelManager_;

	TextureManager* textureManager_;
	ImGuiManager* imGuiManager_;
	Input* input_;
	Audio* audio_;
	SoundData soundData1_;
};