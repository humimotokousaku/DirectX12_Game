#include "Object3D.h"
#include "PipelineManager.h"
#include "ModelManager.h"

Object3D::~Object3D() {
	worldTransform.constBuff_.ReleaseAndGetAddressOf();
	skinCluster_.clear();
	animation_.clear();
}

void Object3D::Initialize() {
	worldTransform.Initialize();
}

void Object3D::Draw(uint32_t textureNum) {
	// カメラ
	if (camera_) {
		//camera_->Update();
	}

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	/// コマンドを積む

	// スキンクラスタが存在している場合
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		SkinningPSO::GetInstance()->SetCommand();
		// アニメーション
		if (model_->animation_.isActive) {
			AnimationUpdate(model_->skinCluster_, model_->skeleton_, model_->animation_, animationTime_);
		}
	}
	else {
		// 使用するPSO
		Object3dPSO::GetInstance()->SetCommand();
		// スキンクラスターはないがアニメーションがある
		if (model_->animation_.isActive) {
			animationTime_ += 1.0f / 60.0f * model_->animation_.playBackSpeed;
			// ループ再生の場合
			if (model_->animation_.isLoop) {
				// 通常
				if (model_->animation_.playBackSpeed >= 0.0f) {
					animationTime_ = Custom_fmod(animationTime_, model_->animation_.duration, 0);
				}
				// 逆再生
				else if (model_->animation_.playBackSpeed < 0.0f) {
					animationTime_ = Custom_fmod(animationTime_, model_->animation_.duration, model_->animation_.duration);
				}
			}

			// アニメーション再生
			NodeAnimation& rootNodeAnimation = model_->animation_.nodeAnimations[model_->GetModelData().rootNode.name];
			Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateScaleValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);
			worldTransform.matWorld_ = Multiply(localMatrix, worldTransform.matWorld_);
		}
		worldTransform.TransferMatrix();
	}

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());

	// 見た目を描画
	model_->Draw(camera_->GetViewProjection(), textureNum);
}

void Object3D::Draw() {
	// カメラ
	if (camera_) {
		//camera_->Update();
	}

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	/// コマンドを積む

	// スキンクラスタが存在している場合
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		SkinningPSO::GetInstance()->SetCommand();
		// アニメーション
		if (model_->animation_.isActive) {
			AnimationUpdate(model_->skinCluster_, skeleton_, model_->animation_, animationTime_);
		}
	}
	else {
		// 使用するPSO
		Object3dPSO::GetInstance()->SetCommand();
		// スキンクラスターはないがアニメーションがある
		if (model_->animation_.isActive) {
			animationTime_ += 1.0f / 60.0f * model_->animation_.playBackSpeed;
			// ループ再生の場合
			if (model_->animation_.isLoop) {
				// 通常
				if (model_->animation_.playBackSpeed >= 0.0f) {
					animationTime_ = Custom_fmod(animationTime_, model_->animation_.duration, 0);
				}
				// 逆再生
				else if (model_->animation_.playBackSpeed < 0.0f) {
					animationTime_ = Custom_fmod(animationTime_, model_->animation_.duration, model_->animation_.duration);
				}
			}

			// アニメーション再生
			NodeAnimation& rootNodeAnimation = model_->animation_.nodeAnimations[model_->GetModelData().rootNode.name];
			Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateScaleValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);
			worldTransform.matWorld_ = Multiply(localMatrix, worldTransform.matWorld_);
		}
		worldTransform.TransferMatrix();
	}

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());

	// 見た目を描画
	model_->Draw(camera_->GetViewProjection());
}

void Object3D::CurrentAnimUpdate() {
	// 現在使用しているアニメーションを検出
	for (int i = 0; i < animation_.size(); i++) {
		// 使用しているアニメーションをモデルデータに代入
		if (animation_[i].isActive) {
			model_->animation_ = animation_[i];
			model_->skinCluster_ = skinCluster_[i];
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
					model_->animation_.isActive = false;
				}
			}
		}
	}
}

void Object3D::ImGuiParameter(const char* name) {
	int index{};
	ImGui::Begin(name);
	for (int i = 0; i < animation_.size(); i++) {
		// アニメーションの名前を設定していないとき
		if (animation_[i].name[0] == '\0') {
			std::string beginName = name + std::to_string(index);
			if (ImGui::TreeNode(beginName.c_str())) {
				ImGui::DragFloat3("translation", &worldTransform.translate.x, 0.01f, -100, 100);
				ImGui::DragFloat3("scale", &worldTransform.scale.x, 0.01f, -100, 100);
				ImGui::DragFloat3("rotate", &worldTransform.rotate.x, 0.01f, -6.28f, 6.28f);
				ImGui::DragFloat("playBackSpeed", &animation_[i].playBackSpeed, 0.01f, -10.0f, 10.0f);
				ImGui::Checkbox("isAnimation", &animation_[i].isActive);
				ImGui::DragFloat("duration", &animation_[i].duration, 0, -10.10);
				ImGui::TreePop();
				index++;
			}
		}
		// アニメーションに名前を設定しているとき
		else {
			if (ImGui::TreeNode(animation_[i].name)) {
				ImGui::DragFloat3("translation", &worldTransform.translate.x, 0.01f, -100, 100);
				ImGui::DragFloat3("scale", &worldTransform.scale.x, 0.01f, -100, 100);
				ImGui::DragFloat3("rotate", &worldTransform.rotate.x, 0.01f, -6.28f, 6.28f);
				ImGui::DragFloat("playBackSpeed", &animation_[i].playBackSpeed, 0.01f, -10.0f, 10.0f);
				ImGui::Checkbox("isAnimation", &animation_[i].isActive);
				ImGui::DragFloat("duration", &animation_[i].duration, 0, -10.10);
				ImGui::TreePop();
			}
		}
	}

	ImGui::DragFloat("animTime", &animationTime_, 0, -100, 100);
	ImGui::End();
}
