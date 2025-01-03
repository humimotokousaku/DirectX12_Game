#include "Particles.h"
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include "DirectionalLight.h"
#include "SrvManager.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>
#include <numbers>
#include <vector>

Particles::Particles() {
	emitter_.transform.Initialize();
}

Particles::~Particles() {

}

void Particles::Initialize(const Vector3& emitterPos) {
	// 頂点の座標
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} }); // 左上
	modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} }); // 右上
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} }); // 左下
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} }); // 左下
	modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} }); // 右上
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} }); // 右下

	// Resource作成
	instancingResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleForGPU) * kNumMaxInstance);
	instancingData_ = nullptr;
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	// SRVの作成
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	instancingSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(srvIndex_, instancingResource_.Get(), kNumMaxInstance, sizeof(ParticleForGPU));


	// Dissolveの情報を書き込む
	dissolveResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DissolveDataForGPU)).Get();
	// 書き込むためのアドレスを取得
	dissolveResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData_));
	dissolveData_->isActive = true;
	dissolveData_->maskThreshold = 0.5f;


	// 頂点データのメモリ確保
	CreateVertexResource();
	CreateVertexBufferView();

	// マテリアルのメモリ確保
	CreateMaterialResource();

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();

	// エミッターの設定
	emitter_.count = 10;
	emitter_.frequency = 1;
	emitter_.frequencyTime = 0.0f;
	emitter_.transform.translate = emitterPos;

	// フィールド(疑似風を作成)
	accField_.acc = { 15,0,0 };
	accField_.area.min = { -10,-10,-10 };
	accField_.area.max = { 10,10,10 };
	accField_.isActive = true;

	// パーティクルの詳細設定
	particle_ = MakeNewParticle(emitter_.transform.translate);

	// パーティクルの消え方
	easeFunc_ = Easings::EaseInCubic;
}

void Particles::Update() {
	emitter_.transform.UpdateMatrix();

	uint32_t numInstance = 0;
	for (std::list<Particle>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();) {
		// 生存時間が過ぎたら処理を行わない
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}

		if (numInstance < kNumMaxInstance) {
			// パーティクルの挙動
			if (updateFunc_) {
				updateFunc_(*particleIterator);
			}
			// 移動処理
			(*particleIterator).transform.translate = Add((*particleIterator).transform.translate, Multiply(kDeltaTime, (*particleIterator).vel));

			// 指定した時間に透明になる
			float alpha = 1.0f - easeFunc_((*particleIterator).currentTime / (*particleIterator).lifeTime);
			(*particleIterator).color.w = alpha;
			instancingData_[numInstance].color = (*particleIterator).color;
			++numInstance;
		}
		++particleIterator;
	}

	emitter_.frequencyTime += kDeltaTime;
	if (emitter_.spawnLeft <= emitter_.spawnCount || emitter_.spawnCount <= 0) {
		if (emitter_.frequency <= emitter_.frequencyTime) {
			std::random_device seedGenerator;
			std::mt19937 randomEngine(seedGenerator());
			particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
			emitter_.frequencyTime -= emitter_.frequency;
			if (emitter_.spawnCount != 0) {
				emitter_.spawnLeft++;
			}
		}
	}
}

void Particles::Draw() {
	// カメラ
	if (camera_) {
		camera_->Update();
	}

	// カメラ行列
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	// 板ポリを正面に向ける
	Matrix4x4 backToFrontMatrix = MakeIdentity4x4();
	// billboardMatrixを作成
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	uint32_t numInstance = 0;
	for (std::list<Particle>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();) {
		// 生存時間が過ぎたら処理を行わない
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}

		if (numInstance < kNumMaxInstance) {
			// WVPとworldMatrixの計算
			Matrix4x4 worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale, billboardMatrix, (*particleIterator).transform.translate + emitter_.transform.worldPos);
			instancingData_[numInstance].World = Multiply(worldMatrix, Multiply(camera_->GetViewProjection().matView, camera_->GetViewProjection().matProjection));
			instancingData_[numInstance].WVP = instancingData_[numInstance].World;
			++numInstance;

			// 時間を進める
			(*particleIterator).currentTime += kDeltaTime;
		}

		++particleIterator;
	}

	// コマンドを積む
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	PipelineManager::GetInstance()->SetParticlePSO();
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// DescriptorTableの設定
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(1, srvIndex_);
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textures_.particle);
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(5, textures_.dissolve);
	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, dissolveResource_->GetGPUVirtualAddress());

	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(6, numInstance, 0, 0);
}

bool Particles::IsCollision(const AABB& aabb, const Vector3& point) {
	if ((aabb.min.x <= point.x && aabb.max.x >= point.x) &&
		(aabb.min.y <= point.y && aabb.max.y >= point.y) &&
		(aabb.min.z <= point.z && aabb.max.z >= point.z)
		) {
		return true;
	}
	return false;
}

Particle Particles::MakeNewParticle(std::mt19937& randomEngine) {
	Particle particle{};
	// 座標
	std::uniform_real_distribution<float> distribution(randomTranslateLimit.min, randomTranslateLimit.max);
	Vector3 randomTranslate = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	particle.transform.translate = /*translate + */randomTranslate;
	// 速度
	std::uniform_real_distribution<float> distVelX(randomVelLimit[0].min, randomVelLimit[0].max);
	std::uniform_real_distribution<float> distVelY(randomVelLimit[1].min, randomVelLimit[1].max);
	std::uniform_real_distribution<float> distVelZ(randomVelLimit[2].min, randomVelLimit[2].max);

	particle.vel = { distVelX(randomEngine) ,distVelY(randomEngine) ,distVelZ(randomEngine) };
	particle.vel += particle_.vel;
	// 大きさ
	std::uniform_real_distribution<float> distScale(randomScaleLimit.min, randomScaleLimit.max);
	particle.transform.scale = { distScale(randomEngine),distScale(randomEngine) ,distScale(randomEngine) };
	particle.transform.scale += particle_.transform.scale;
	// 色
	std::uniform_real_distribution<float> distColor(randomColorLimit.min, randomColorLimit.max);
	particle.color = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine),1.0f };
	particle.color.x += particle_.color.x;
	particle.color.y += particle_.color.y;
	particle.color.z += particle_.color.z;
	// 生存可能時間
	std::uniform_real_distribution<float> distTime(randomLifeTimeLimit.min, randomLifeTimeLimit.max);
	particle.lifeTime = distTime(randomEngine);
	particle.lifeTime += particle_.lifeTime;

	particle.transform.rotate = { 0,0,0 };
	particle.currentTime = 0;

	return particle;
}

Particle Particles::MakeNewParticle(const Vector3& translate) {
	Particle particle{};
	particle.color.w = 1.0f;
	particle.lifeTime = 1;
	particle.currentTime = 0;
	particle.transform.translate = translate;

	return particle;
}

std::list<Particle> Particles::Emission(const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<Particle> particles;

	for (int32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(randomEngine));
	}
	return particles;
}

Vector3 Particles::KelvinToRGB(int kelvin) {
	Vector3 color{};

	float temp = kelvin / 100.0f;
	float red, green, blue;

	if (temp <= 66) {
		red = 255;
		green = temp;
		green = 99.4708025861f * log(green) - 161.1195681661f;

		if (temp <= 19)
			blue = 0;
		else {
			blue = temp - 10;
			blue = 138.5177312231f * log(blue) - 305.0447927307f;
		}
	}
	else {
		red = temp - 60;
		red = 329.698727446f * pow(red, -0.1332047592f);

		green = temp - 60;
		green = 288.1221695283f * pow(green, -0.0755148492f);

		blue = 255;
	}

	color.x = red / 255.0f;
	color.y = green / 255.0f;
	color.z = blue / 255.0f;

	return color;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Particles::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
	HRESULT hr;
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれからは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// 実際に頂点リソースを作る
	hr = device.Get()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

void Particles::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * modelData_.vertices.size()).Get();
}

void Particles::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * UINT(modelData_.vertices.size());
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Particles::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material) * kNumMaxInstance).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}
