#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "ModelManager.h"
#include "PipelineManager.h"
#include "Dissolve.h"
#include "Collider.h"

class IObject3D {
public:
	// デストラクタ
	virtual ~IObject3D() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="textureNum">テクスチャ番号</param>
	virtual void Draw(uint32_t textureNum, int fillMode = kFillModeSolid) = 0;
	/// <summary>
	/// 描画処理(モデルで使われているテクスチャを使用する)
	/// </summary>
	virtual void Draw(int fillMode = kFillModeSolid) = 0;

#pragma region Getter
	// モデル
	Model* GetModel() { return model_; }
	// カメラを取得
	Camera* GetCamera() { return camera_; }
#pragma endregion

#pragma region Setter
	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	// モデルのセット
	void SetModel(const std::string& directoryPath, const std::string& filePath);
	void SetModel(Model* model);

	/// <summary>
	/// 当たり判定をとるかを設定
	/// </summary>
	/// <param name="isCollision">当たり判定</param>
	void SetIsCollision(const bool& isCollision) { collider->SetIsActive(isCollision); }

#pragma region アニメーション
	// アニメーション追加
	void AddAnimation(const std::string& directoryPath, const std::string& filePath, const char* animName = "\0") {
		// モデルをセット
		model_ = ModelManager::GetInstance()->FindModel(directoryPath, filePath);
		// アニメーションデータを代入
		Motion animation = model_->GetAnimation();
		// アニメーションの名前を代入
		animation.name = animName;
		animation_.push_back(animation);

		// 新しくリソースを作成
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
	void AddAnimation(Model* model, const char* animName = "\0") {
		// アニメーションデータを代入
		Motion animation = model->GetAnimation();
		// アニメーションの名前を代入
		animation.name = animName;
		animation_.push_back(animation);

		// 新しくリソースを作成
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model->GetModelData());
		skinCluster_.push_back(skinCluster);
	}

	// アニメーションの名前を設定
	void SetAnimName(const char* animName) {
		animation_[0].name = animName;
		model_->SetAnimationName(animation_[0].name);
	}

	// 指定したアニメーションを再生(ただしその他のアニメーションを強制的に止めてしまうので毎フレーム呼び出すのは禁止)
	void StartAnim(const char* animName) {
		for (int i = 0; i < animation_.size(); i++) {
			if (animation_[i].name == animName) {
				animation_[i].time = 0.0f;
				animation_[i].isActive = true;
				model_->SetAnimation(animation_[i]);
				if (model_->GetModelData().isSkinClusterData) {
					model_->SetSkinCluster(skinCluster_[i]);
				}
			}
			else {
				animation_[i].isActive = false;
			}
		}
	}
	void StartAnim(const int index) {
		for (int i = 0; i < animation_.size(); i++) {
			if (i == index) {
				animation_[i].time = 0.0f;
				animation_[i].isActive = true;
				model_->SetAnimation(animation_[i]);
				if (model_->GetModelData().isSkinClusterData) {
					model_->SetSkinCluster(skinCluster_[i]);
				}
			}
			else {
				animation_[i].isActive = false;
			}
		}
	}
	// 全てのアニメーションを停止
	void EndAnim() {
		for (int i = 0; i < animation_.size(); i++) {
			animation_[i].isActive = false;
			model_->SetIsAnimation(false);
		}
	}
	// アニメーションのループ設定
	void SetIsAnimationLoop(bool isActive) { model_->SetIsAnimationLoop(isActive); }
	/// <summary>
	/// アニメーションの再生速度
	/// 2:二倍速
	/// -:逆再生
	/// </summary>
	/// <param name="speed">再生倍率</param>
	void SetAnimationSpeed(float speed) { model_->SetAnimationSpeed(speed); }
#pragma endregion

	// ライティングの設定
	void SetIsLighting(const bool& isActive) { model_->SetIsLighting(isActive); }
	// 鏡面反射の輝度の設定
	void SetShininess(const float& shininess) { model_->SetShininess(shininess); }
	// 色の設定
	void SetColor(const Vector4& RGBA) { model_->GetMaterialData()->color = RGBA; }
	// α値の設定
	void SetAlpha(const float& alpha) { model_->GetMaterialData()->color.w = alpha; }
	// UV座標の設定
	void SetUVTranslate(const Vector3& translate) { model_->uvTransform.translate = translate; }
	// UVの角度の設定
	void SetUVRotate(const Vector3& rotate) { model_->uvTransform.rotate = rotate; }
	// UVのスケールの設定
	void SetUVScale(const Vector3& scale) { model_->uvTransform.scale = scale; }

	// dissolve用のテクスチャを設定
	void SetDissolveTexture(uint32_t dissolveTexture) { model_->SetDissolveTexture(dissolveTexture); }

	// 描画をするかの設定
	void SetIsActive(const bool& isActive) { isActive_ = isActive; }
#pragma endregion

public:// パブリックな変数
	// ワールドトランスフォーム
	WorldTransform worldTransform;

	// 衝突判定(初期設定は判定をとらない)
	std::unique_ptr<Collider> collider;

protected:
	// カメラ
	Camera* camera_;

	// モデル
	Model* model_;

	// アニメーション
	std::vector<Motion> animation_;
	float animationTime_ = 0.0f;

	// スキンクラスタ
	std::vector<SkinCluster> skinCluster_;

	// スケルトン
	Skeleton skeleton_;

	// 描画するか
	bool isActive_ = true;
};