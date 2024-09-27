#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "ModelManager.h"
#include "Sphere.h"
#include "PipelineManager.h"
#include "Collider.h"

class Object3D {
public:
	// コンストラクタ
	Object3D() = default;
	// デストラクタ
	~Object3D();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// 更新処理
	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="viewProjection">カメラ</param>
	/// <param name="textureNum">テクスチャ番号</param>
	void Draw(uint32_t textureNum, int fillMode = kFillModeSolid);
	/// <summary>
	/// 描画処理(モデルで使われているテクスチャを使用する)
	/// </summary>
	void Draw(int fillMode = kFillModeSolid);

	/// <summary>
	/// 今使用しているアニメーションの検出と更新
	/// </summary>
	void CurrentAnimUpdate();

	///
	/// User Method
	/// 

	// ImGuiを表示
	void ImGuiParameter(const char* name);

	/// <summary>
	/// モデルの頂点位置を確認するImGuiを起動
	/// </summary>
	void CheckVertex();

#pragma region Getter
	// モデル
	Model* GetModel() { return model_; }
#pragma endregion

#pragma region Setter
	// カメラ
	void SetCamera(Camera* camera) { 
		camera_ = camera; 
		sphere_->SetCamera(camera_);
	}

	// モデルのセット
	void SetModel(const std::string& directoryPath, const std::string& filePath);
	void SetModel(Model* model);

#pragma region アニメーション
	// アニメーション追加
	void AddAnimation(const std::string& directoryPath, const std::string& filePath, const char* animName = "\0") {
		// モデルをセット
		model_ = ModelManager::GetInstance()->FindModel(directoryPath, filePath);
		// アニメーションデータを代入
		Motion animation = model_->animation_;
		// アニメーションの名前を代入
		animation.name = animName;
		animation_.push_back(animation);

		// 新しくリソースを作成
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
	void AddAnimation(Model* model, const char* animName = "\0") {
		// アニメーションデータを代入
		Motion animation = model->animation_;
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
		model_->animation_.name = animation_[0].name;
	}

	// 指定したアニメーションを再生(ただしその他のアニメーションを強制的に止めてしまうので毎フレーム呼び出すのは禁止)
	void StartAnim(const char* animName) {
		for (int i = 0; i < animation_.size(); i++) {
			if (animation_[i].name == animName) {
				animation_[i].time = 0.0f;
				animation_[i].isActive = true;
				model_->animation_ = animation_[i];
				if (model_->GetModelData().isSkinClusterData) {
					model_->skinCluster_ = skinCluster_[i];
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
				model_->animation_ = animation_[i];
				if (model_->GetModelData().isSkinClusterData) {
					model_->skinCluster_ = skinCluster_[i];
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
			model_->animation_.isActive = false;
		}
	}
	// アニメーションのループ設定
	void SetIsAnimLoop(bool isActive) { model_->animation_.isLoop = isActive; }
	/// <summary>
	/// アニメーションの再生速度
	/// 2:二倍速
	/// -:逆再生
	/// </summary>
	/// <param name="speed">再生倍率</param>
	void SetAnimSpeed(float speed) { model_->animation_.playBackSpeed = speed; }
#pragma endregion

	void SetIsEditor(bool isActive) { isEditor_ = isActive; }
	// ライティングの設定
	void SetIsLighting(bool isActive) { model_->SetIsLighting(isActive); }
	// 鏡面反射の輝度の設定
	void SetShininess(float shininess) { model_->SetShininess(shininess); }
	// 色の設定
	void SetColor(Vector4 RGBA) { model_->materialData_->color = RGBA; }
	// 描画をするかの設定
	void SetIsActive(bool isActive) { isActive_ = isActive; }
#pragma endregion

public:// パブリックな変数
	// ワールドトランスフォーム
	WorldTransform worldTransform;
	// 衝突判定(初期設定は判定をとらない)
	std::unique_ptr<Collider> collider;

private:// プライベートな変数
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

	// 選択された頂点表示用の球体
	std::unique_ptr<Sphere> sphere_;
	// エディターモード起動
	bool isEditor_ = false;
	int selectVertex_ = -1;
	std::vector<const char*> verteciesName;
	const char* selectedVertexName;

	// 描画するか
	bool isActive_ = true;
};