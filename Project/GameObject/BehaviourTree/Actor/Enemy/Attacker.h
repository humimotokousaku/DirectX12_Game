#pragma once

#include "../Actor.h"
#include "../IAgent.h"
#include "../ActorHealth.h"
#include "../../CompositeNode/Selector.h"

#include "../../Math/MyTimer.h"
#include "Object3d.h"
#include "Camera.h"

class IWorld;
class Attacker : public Actor, public IAgent {
	const float cCoolTime = 60.f;

public:
	//コンストラクタ
	Attacker(IWorld* world, Camera* camera, std::string behavior_tree_file_path);
	//デストラクタ
	~Attacker();
	//更新
	void update(float delta_time);
	//描画
	void draw() const;
	//半透明の描画
	void draw_transparent() const;
	//GUIの描画
	void draw_gui() const;

	// 衝突リアクション
	//void react(Actor& other) override;

	// BehaviorTreeの中で現在動いているノードのIDを取得
	const int GetBTRunningNodeID() const;

public:
	const Vector2& get_position() const override;
	void move_towards(const Vector2& target, float speed) override;

	const float get_health() const override;
	void attack() override;

private:
	void move(float delta_time);

private:
	INode* mpBehaviourTree = nullptr;
	std::unique_ptr<Object3D> object_;
};
