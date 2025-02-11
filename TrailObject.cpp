#include "TrailObject.h"

TrailObject::TrailObject(int bufferSize) {
	bufferSize;
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
	//CreateCurveVertex(usedPosArray);

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