#pragma once
#include "../BehaviourTree/Actor/Actor.h"
#include "Input.h"
#include "Object3D.h"

class Player : public Actor{
//const float cCoolTime = 60.f;
public:
	// コンストラクタ
	Player(Camera* camera);

	// デストラクタ
	~Player();

	// 更新
	void update(float delta_time) override;

	// 描画
	void draw() const override;

	//GUIの描画
	void draw_gui() override;

	void InputHandle();
	/// <summary>
	/// 戦車挙動
	/// </summary>
	/// <param name="leftStickY"></param>
	/// <param name="rightStickY"></param>
	/// <param name="deltaTime"></param>
	void DifferentialUpdate(float leftStickY, float rightStickY, float deltaTime);

private:
	//float mSpeed = 4.f;
	Input* input_;
	// ゲームパッド
	XINPUT_STATE joyState_;

private:
	std::unique_ptr<Object3D> obj_;

	float treadWidth = 2.5f;
	float maxSpeed = 0.5f;
	float x = 0.0f, z = 0.0f, angle = 0.0f, omega;
	float vL = 0.0f, vR = 0.0f;
};