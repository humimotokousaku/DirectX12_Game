#pragma once
#include "IPart.h"

class WeakPoint : public IPart {
public:
	~WeakPoint() override = default;
	void Initialize(const WorldTransform* parent, Camera* camera, Model* model, int id) override;
	void Update() override;
	void Draw() override;
	// 衝突応答
	void OnCollision(Collider* collider) override;

private:

};