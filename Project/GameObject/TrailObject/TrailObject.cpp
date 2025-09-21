#include "TrailObject.h"
#include <DirectXMath.h>

TrailObject::TrailObject(int bufferSize, Camera* camera) {
	// エンジン機能のインスタンスを入れる
	dxCommon_ = DirectXCommon::GetInstance();
	texManager_ = TextureManager::GetInstance();
	posArray_.resize(bufferSize);

	camera_ = camera;
}

void TrailObject::Update() {
	// トレイルの頂点を後ろから変更
	for (size_t i = posArray_.size() - 1; i > 0; --i) {
		posArray_[i] = posArray_[i - 1];
	}
	// 先頭の情報がないので現在のトレイル情報を代入
	posArray_.front() = currentPos_;
	currentPos_ = TrailData();

	//曲線を作る
	std::vector<TrailData> usedPosArray = GetUsedPosArray();
	if (usedPosArray.empty())return;
	CreateCurveVertex(usedPosArray, 16);

	//頂点データを更新する
	float amount = 1.0f / (usedPosArray.size() - 1);
	float v = 0;
	vertex_.clear();
	vertex_.resize(usedPosArray.size() * 2);
	for (size_t i = 0, j = 0; i < vertex_.size() && j < usedPosArray.size(); i += 2, ++j) {
		vertex_[i].pos = usedPosArray[j].head;
		vertex_[i].uv = Vector2(1.0f, v);
		vertex_[i + 1].pos = usedPosArray[j].tail;
		vertex_[i + 1].uv = Vector2(0.0f, v);
		v += amount;
	}
}

void TrailObject::Draw() {
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	// material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureNum_);
	// viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, camera_->viewProjection_.constBuff_->GetGPUVirtualAddress());

	dxCommon_->GetCommandList()->DrawIndexedInstanced(split_ * 6, 1, 0, 0, 0);
}

void TrailObject::CreateCurveVertex(std::vector<TrailData>& usedPosArray, int split) {
	if (usedPosArray.size() < 3 || split < 1) { return; }
	// トレイルの分割数を指定
	split_ = split;

	// 動的に頂点リストを作成
	std::vector<TrailData> newPosArray;
	newPosArray.reserve(usedPosArray.size() + (usedPosArray.size() - 1) * split);
	const float amount = 1.0f / (split + 1);

	// 頂点を一つずつ作成していく
	TrailData newPos;
	newPosArray.push_back(usedPosArray.front());
	for (size_t i = 0; i < usedPosArray.size() - 1; ++i) {
		float ratio = amount;
		//	CatMulに使う4つの点を作る（p0, p3がない時の処理も書く）
		Vector3 p0Head = i == 0 ? ((usedPosArray[1].head) + (usedPosArray[2].head)) * 0.5f : (usedPosArray[i - 1].head);
		Vector3 p1Head = usedPosArray[i].head;
		Vector3 p2Head = usedPosArray[i + 1].head;
		Vector3 p3Head = i == usedPosArray.size() - 2 ? (p0Head + p2Head) * 0.5f : (usedPosArray[i + 2].head);

		Vector3 p0Tail = i == 0 ? ((usedPosArray[1].tail) + (usedPosArray[2].tail)) * 0.5f : (usedPosArray[i - 1].tail);
		Vector3 p1Tail = (usedPosArray[i].tail);
		Vector3 p2Tail = (usedPosArray[i + 1].tail);
		Vector3 p3Tail = i == usedPosArray.size() - 2 ? (p0Tail + p2Tail) * 0.5f : (usedPosArray[i + 2].tail);

		for (size_t j = 0; j < static_cast<size_t>(split - 1); ++j) {
			newPos = TrailData();

			newPos.isUsed = true;
			newPos.head = Lerps::CatmullRomSpline(p0Head, p1Head, p2Head, p3Head, ratio);
			newPos.tail = Lerps::CatmullRomSpline(p0Tail, p1Tail, p2Tail, p3Tail, ratio);

			newPosArray.push_back(newPos);
			ratio += amount;
		}
		newPosArray.push_back(usedPosArray[i + 1]);
	}
	usedPosArray = newPosArray;
}

void TrailObject::SetPos(const Vector3& head, const Vector3& tail) {
	currentPos_.head = head;
	currentPos_.tail = tail;
	currentPos_.isUsed = true;
}

std::vector<TrailData> TrailObject::GetUsedPosArray() {
	std::vector<TrailData> result;
	for (int i = 0; i < posArray_.size(); i++) {
		result.push_back(posArray_[i]);
	}

	return result;
}

void TrailObject::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TrailBuffer) * 4 * split_).Get();

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(TrailBuffer) * 4 * split_;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(TrailBuffer);

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertex_));
	std::memcpy(vertex_.data(), vertex_.data(), sizeof(TrailBuffer) * 4 * split_);
}

void TrailObject::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	// Lightingするか
	materialData_->enableLighting = true;
	materialData_->shininess = 70.0f;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
}

void TrailObject::CreateIndexResource() {
	// インデックスのリソース作成
	indexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6 * split_);
	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertex_));

	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * 4 * split_);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}