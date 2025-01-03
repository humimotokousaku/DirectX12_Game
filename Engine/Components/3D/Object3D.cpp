#include "Object3D.h"
#include "PipelineManager.h"
#include "ModelManager.h"
#include <array>
#include <Utility.h>

Object3D::~Object3D() {
	worldTransform.constBuff_.ReleaseAndGetAddressOf();
	skinCluster_.clear();
	animation_.clear();
	delete model_;
}

void Object3D::Initialize() {
	// モデル生成
	worldTransform.Initialize();
	model_ = new Model();

	// 衝突判定
	collider = std::make_unique<Collider>();
	collider->worldTransform.parent_ = &worldTransform;
	collider->SetIsActive(false);
}

void Object3D::Draw(uint32_t textureNum, int fillMode) {
	if (!isActive_) { return; }

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	/// コマンドを積む

	// スキンクラスタが存在している場合
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		PipelineManager::GetInstance()->SetSkinningPSO(fillMode);
		// アニメーション
		if (model_->GetAnimation().isActive) {
			model_->AnimationUpdate(animationTime_);
		}
	}
	else {
		// 使用するPSO
		PipelineManager::GetInstance()->SetObject3dPSO(fillMode);
		// スキンクラスターはないがアニメーションがある
		if (model_->GetAnimation().isActive) {
			animationTime_ += 1.0f / 60.0f * model_->GetAnimation().playBackSpeed;
			// ループ再生の場合
			if (model_->GetAnimation().isLoop) {
				// 通常
				if (model_->GetAnimation().playBackSpeed >= 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->GetAnimation().duration, 0);
				}
				// 逆再生
				else if (model_->GetAnimation().playBackSpeed < 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->GetAnimation().duration, model_->GetAnimation().duration);
				}
			}

			// アニメーション再生
			NodeAnimation& rootNodeAnimation = model_->GetAnimation().nodeAnimations[model_->GetModelData().rootNode.name];
			Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateScaleValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = MakeAffineMatrix_Quaternion(scale, rotate, translate);
			worldTransform.matWorld_ = Multiply(localMatrix, worldTransform.matWorld_);
		}
		worldTransform.TransferMatrix();
	}

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());

	// 見た目を描画
	model_->Draw(camera_->GetViewProjection(), textureNum);
}

void Object3D::Draw(int fillMode) {
	if (!isActive_) { return; }

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	/// コマンドを積む

	// スキンクラスタが存在している場合
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		PipelineManager::GetInstance()->SetSkinningPSO(fillMode);
		// アニメーション
		if (model_->GetAnimation().isActive) {
			model_->AnimationUpdate(animationTime_);
		}
	}
	else {
		// 使用するPSO
		PipelineManager::GetInstance()->SetObject3dPSO(fillMode);
		// スキンクラスターはないがアニメーションがある
		if (model_->GetAnimation().isActive) {
			animationTime_ += 1.0f / 60.0f * model_->GetAnimation().playBackSpeed;
			// ループ再生の場合
			if (model_->GetAnimation().isLoop) {
				// 通常
				if (model_->GetAnimation().playBackSpeed >= 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->GetAnimation().duration, 0);
				}
				// 逆再生
				else if (model_->GetAnimation().playBackSpeed < 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->GetAnimation().duration, model_->GetAnimation().duration);
				}
			}

			// アニメーション再生
			NodeAnimation& rootNodeAnimation = model_->GetAnimation().nodeAnimations[model_->GetModelData().rootNode.name];
			Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateScaleValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = MakeAffineMatrix_Quaternion(scale, rotate, translate);
			worldTransform.matWorld_ = Multiply(localMatrix, worldTransform.matWorld_);
		}
		worldTransform.TransferMatrix();
	}

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());

	// 見た目を描画
	model_->Draw(camera_->GetViewProjection());
}

void Object3D::ImGuiParameter(const char* name) {
	int index{};
	ImGui::Begin(name);
	if (ImGui::BeginTabBar(name)) {
		if (ImGui::BeginTabItem("Base")) {
			for (int i = 0; i < animation_.size(); i++) {
				// アニメーションの名前を設定していないとき
				std::string beginName = name;
				if (animation_[i].name[0] == '\0') {
					beginName += std::to_string(index);
				}

				if (ImGui::TreeNode(beginName.c_str())) {
					ImGui::DragFloat3("translation", &worldTransform.translate.x, 0.01f, -100, 100);
					ImGui::DragFloat3("scale", &worldTransform.scale.x, 0.01f, -100, 100);
					ImGui::DragFloat3("rotate", &worldTransform.rotate.x, 0.01f, -6.28f, 6.28f);
					ImGui::DragFloat("playBackSpeed", &animation_[i].playBackSpeed, 0.01f, -10.0f, 10.0f);
					ImGui::Checkbox("isAnimation", &animation_[i].isActive);
					ImGui::DragFloat("duration", &animation_[i].duration, 0, -10.0f,10.0f);
					ImGui::DragFloat("animTime", &animationTime_, 0, -100, 100);
					ImGui::TreePop();
					index++;
				}
			}
			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar(); // タブバーの終了
	ImGui::End();
}

void Object3D::CurrentAnimUpdate() {
	// 現在使用しているアニメーションを検出
	for (int i = 0; i < animation_.size(); i++) {
		// 使用しているアニメーションをモデルデータに代入
		if (animation_[i].isActive) {
			model_->SetAnimation(animation_[i]);
			model_->SetSkinCluster(skinCluster_[i]);
		}

		int j = 0;
		// 全てのアニメーションが終了しているかを確認
		while (j < animation_.size()) {
			// 一つでも作動しているのならループを抜ける
			if (animation_[j].isActive) {
				break;
			}
			else {
				j++;
				// 全てのアニメーションが停止しているなら止める
				if (j > animation_.size() - 1) {
					model_->SetIsAnimation(false);
				}
			}
		}
	}
}