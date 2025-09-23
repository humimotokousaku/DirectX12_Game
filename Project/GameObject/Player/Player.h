#pragma once
#include "../BehaviourTree/Actor/Actor.h"

class Player : public Actor{
const float cCoolTime = 60.f;

public:
	// コンストラクタ
	Player();

	// デストラクタ
	~Player();

	// 更新
	virtual void update(float delta_time);

	// 描画
	virtual void draw() const;

	//GUIの描画
	virtual void draw_gui() const;

private:
	float mSpeed = 4.f;
};