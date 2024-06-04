#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "ModelManager.h"

class Object3D
{
public:
	///
	/// Default Method
	/// 
	
	// コンストラクタ
	Object3D() = default;
	// デストラクタ
	~Object3D();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="viewProjection">カメラ</param>
	/// <param name="textureNum">テクスチャ番号</param>
	void Draw(uint32_t textureNum);
	/// <summary>
	/// 描画処理(モデルで使われているテクスチャを使用する)
	/// </summary>
	void Draw();

	///
	/// Default Method
	/// 

	void ImGuiParameter(const char* name);

	/// Getter


	/// Setter
	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	// モデルのセット
	void SetModel(const std::string& directoryPath, const std::string& filePath) { 
		model_ = ModelManager::GetInstance()->FindModel(directoryPath, filePath);
		// アニメーション
		animation_.push_back(model_->animation_);
		// スケルトン
		skeleton_ = model_->skeleton_;
		// スキンクラスタ
		skinCluster_ = model_->skinCluster_;
	}
	void SetModel(Model* model) {
		model_ = model;
		// アニメーション
		animation_.push_back(model_->animation_);
		// スケルトン
		skeleton_ = model_->skeleton_;
		// スキンクラスタ
		skinCluster_ = model_->skinCluster_;
	}

	// アニメーション追加
	void AddAnimation(const std::string& directoryPath, const std::string& filePath){ 
		// アニメーション
		model_ = ModelManager::GetInstance()->FindModel(directoryPath, filePath);
		animation_.push_back(model_->animation_);
	}
	void AddAnimation(Model* model) {
		// アニメーション
		animation_.push_back(model->animation_);
	}
	
	// アニメーションの名前を設定
	void SetAnimName(const char* animName) {
		animation_[0].name = animName;
		model_->animation_.name = animation_[0].name;
	}

	// 指定したアニメーションを再生(ただしその他のアニメーションを強制的に止めてしまうので毎フレーム呼び出すのは禁止)
	void StartAnim(const char* animName) {
		for (int i = 0; i < animation_.size(); i++) {
			if (animation_[i].name == animName) {
				animation_[i].isActive = true;
			}
			else {
				animation_[i].isActive = false;
			}
		}
	}

public:
	WorldTransform worldTransform;
	Camera* camera_;
	Model* model_;
private:
	// アニメーション
	std::vector<Motion> animation_;
	float animationTime_ = 0.0f;
	bool isPreAnimActive_;
	// スケルトン
	Skeleton skeleton_;
	// スキンクラスタ
	SkinCluster skinCluster_;
};