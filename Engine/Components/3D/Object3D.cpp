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
	// デバッグ用
	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();

	// 衝突判定
	collider = std::make_unique<Collider>();
	collider->worldTransform.parent_ = &worldTransform;
	collider->SetIsActive(false);
}

void Object3D::Draw(uint32_t textureNum, int fillMode) {
	if (!isActive_) { return; }

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	if (isEditor_) {
		// 選択されている頂点
		sphere_->Draw(kFillModeWireFrame);
	}

	/// コマンドを積む

	// スキンクラスタが存在している場合
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		PipelineManager::GetInstance()->SetSkinningPSO(fillMode);
		// アニメーション
		if (model_->animation_.isActive) {
			AnimationUpdate(model_->skinCluster_, model_->skeleton_, model_->animation_, animationTime_);
		}
	}
	else {
		// 使用するPSO
		PipelineManager::GetInstance()->SetObject3dPSO(fillMode);
		// スキンクラスターはないがアニメーションがある
		if (model_->animation_.isActive) {
			animationTime_ += 1.0f / 60.0f * model_->animation_.playBackSpeed;
			// ループ再生の場合
			if (model_->animation_.isLoop) {
				// 通常
				if (model_->animation_.playBackSpeed >= 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->animation_.duration, 0);
				}
				// 逆再生
				else if (model_->animation_.playBackSpeed < 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->animation_.duration, model_->animation_.duration);
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

void Object3D::Draw(int fillMode) {
	if (!isActive_) { return; }

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	// エディターモード起動中
	if (isEditor_) {
		sphere_->Draw();
	}

	/// コマンドを積む

	// スキンクラスタが存在している場合
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		PipelineManager::GetInstance()->SetSkinningPSO(fillMode);
		// アニメーション
		if (model_->animation_.isActive) {
			AnimationUpdate(model_->skinCluster_, skeleton_, model_->animation_, animationTime_);
		}
	}
	else {
		// 使用するPSO
		PipelineManager::GetInstance()->SetObject3dPSO(fillMode);
		// スキンクラスターはないがアニメーションがある
		if (model_->animation_.isActive) {
			animationTime_ += 1.0f / 60.0f * model_->animation_.playBackSpeed;
			// ループ再生の場合
			if (model_->animation_.isLoop) {
				// 通常
				if (model_->animation_.playBackSpeed >= 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->animation_.duration, 0);
				}
				// 逆再生
				else if (model_->animation_.playBackSpeed < 0.0f) {
					animationTime_ = Utility::Custom_fmod(animationTime_, model_->animation_.duration, model_->animation_.duration);
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
		if (isEditor_) {
			//if (ImGui::BeginTabItem("EditorMode")) {
			//	// Create a ListBox
			//	if (ImGui::ListBox("NodeList", &selectVertex_, verteciesName.data(), verteciesName.size()))
			//	{
			//		selectedVertexName = verteciesName[selectVertex_];
			//	}

			//	if (selectedVertexName) {
			//		// Item was clicked
			//		// Handle the selected item
			//		ImGui::Text("selectedNode: %s", selectedVertexName);
			//		if (ImGui::TreeNode(selectedVertexName)) {
			//			ImGui::DragFloat3("LocalPos", &model_->GetModelData().vertices[selectVertex_].position.x, 0.0f);
			//			ImGui::TreePop();
			//		}
			//		Vector3 offset = {
			//			worldTransform.matWorld_.m[3][0] + model_->GetModelData().vertices[selectVertex_].position.x,
			//			worldTransform.matWorld_.m[3][1] + model_->GetModelData().vertices[selectVertex_].position.y,
			//			worldTransform.matWorld_.m[3][2] + model_->GetModelData().vertices[selectVertex_].position.z
			//		};
			//		sphere_->worldTransform.translate = offset;
			//	}
			//	ImGui::EndTabItem();
			//}
		}
	

	}

	ImGui::EndTabBar(); // タブバーの終了
	ImGui::End();
}

void Object3D::CheckVertex() {
	isEditor_ = true;
}

void Object3D::SetModel(const std::string& directoryPath, const std::string& filePath) {
	*model_ = *ModelManager::GetInstance()->GetModel(directoryPath, filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData;
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	model_->SetMaterialResource(materialResource);
	materialData->enableLighting = true;
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	model_->materialData_ = materialData;

	Motion animation = model_->animation_;
	// アニメーション
	animation_.push_back(animation);
	// スケルトン
	skeleton_ = model_->skeleton_;
	// スキンクラスタがあるなら作る
	if (model_->GetModelData().isSkinClusterData) {
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
}

void Object3D::SetModel(Model* model) {
	*model_ = *model;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData;
	materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	model_->SetMaterialResource(materialResource);
	materialData->enableLighting = true;
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	model_->materialData_ = materialData;

	Motion animation = model_->animation_;
	// アニメーション
	animation_.push_back(animation);
	// スケルトン
	skeleton_ = model_->skeleton_;
	// スキンクラスタがあるなら作る
	if (model_->GetModelData().isSkinClusterData) {
		SkinCluster skinCluster = CreateSkinCluster(skeleton_, model_->GetModelData());
		skinCluster_.push_back(skinCluster);
	}
}
