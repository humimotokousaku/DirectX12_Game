#pragma once
#include "IPart.h"

class LeftHand : public IPart {
public:
	~LeftHand() override = default;
	void Initialize(const WorldTransform* parent, Camera* camera, Model* model, int id) override;
	void Update() override;
	void Draw() override;

private:

};