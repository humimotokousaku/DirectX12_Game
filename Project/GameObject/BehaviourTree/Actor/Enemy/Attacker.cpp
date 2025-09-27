#include "Attacker.h"

#include "../BlackBoard.h"
#include "../../../World/IWorld.h"
#include "../../BehaviourTreeBulider.h"
#include "../../CompositeNode/Sequence.h"
#include "../../DecoratorNode/Inverter.h"
#include "../../LeafNode/AlwaysSuccessLeaf.h"
#include "../../LeafNode/ChasePlayerLeaf.h"
#include "../../LeafNode/WaitLeaf.h"
#include "../../LeafNode/CircleAttackLeaf.h"
#include "../../BranchNode/CheckNearPlayer.h"

Attacker::Attacker(IWorld* world, Camera* camera, std::string behavior_tree_file_path)
{
	mpWorld = world;
	mTag = "EnemyTag";
	mName = "Attacker";
	mAttackPower = 1;

	ModelManager::GetInstance()->LoadModel("Models","block.obj");
	// 3Dオブジェクトの生成
	object_ = std::make_unique<Object3D>();
	object_->Initialize();
	object_->SetModel(ModelManager::GetInstance()->FindModel("Models", "block.obj"));
	object_->SetCamera(camera);

	mpBlackBoard = new BlackBoard();
	mpBlackBoard->set_value<IAgent*>("Agent", this);

	auto* p_player = mpWorld->find_actor("Player");
	if (p_player == nullptr) {
		return;
	}
	Vector2 pos = p_player->position();
	mpBlackBoard->set_value<Vector2>("PlayerPos", pos);

	//mpBehaviourTree = BehaviourTreeBuilder::BuildAttackerTree(behavior_tree_file_path, mpBlackBoard);
	auto chase_inverter = new Inverter(mpBlackBoard, new ChasePlayerLeaf(mpBlackBoard));

	auto root_sequence = new Sequence(mpBlackBoard);
	root_sequence->AddNode(new CheckNearPlayer(mpBlackBoard, new AlwaysSuccessLeaf(mpBlackBoard), chase_inverter, 100.f));
	root_sequence->AddNode(new CircleAttackLeaf(mpBlackBoard));
	root_sequence->AddNode(new WaitLeaf(mpBlackBoard, 60.f));
	mpBehaviourTree = root_sequence;
	mpBehaviourTree->Init();

	Vector2 min = mPosition - Vector2{ 30.f, 30.f };
	Vector2 max = mPosition + Vector2{ 30.f, 30.f };
	mCollider = MyRectangle{ min, max };

	mCoolTimer.reset(cCoolTime);
}

Attacker::~Attacker()
{
	delete(mpBehaviourTree);
}

void Attacker::update(float delta_time)
{
	if (!mIsEnableCollider) {
		mCoolTimer.update(delta_time);

		if (mCoolTimer.is_end()) {
			mIsEnableCollider = true;
		}
	}

	auto* p_player = mpWorld->find_actor("Player");

	if (p_player == nullptr) {
		return;
	}

	Vector2 pos = p_player->position();
	mpBlackBoard->set_value<Vector2>("PlayerPos", pos);

	mpBehaviourTree->Tick();

	move(delta_time);

	mVelocity = Vector2(0.0f, 0.0f);
}

void Attacker::draw() const
{
	object_->Draw();
	//DrawCircle(mPosition.x, mPosition.y, 32, GetColor(255, 0, 0), TRUE);
	//mHealthBar.draw();
}

void Attacker::draw_transparent() const
{
}

void Attacker::draw_gui() const
{
	mCollider.draw_debug();
}

//void Attacker::react(Actor& other)
//{
//	if (other.tag() == "PlayerTag") {
//		damage(other.attack_power());
//	}
//}

const int Attacker::GetBTRunningNodeID() const
{
	return mpBehaviourTree->GetRunningNodeID();
}

const Vector2& Attacker::get_position() const
{
	return mPosition;
}

void Attacker::move_towards(const Vector2& target, float speed)
{
	mVelocity = target * speed;
}

const float Attacker::get_health() const
{
	return mHealth.GetHealth();
}

void Attacker::attack()
{
	//Vector2 min, max;

	//min = mPosition - Vector2{ 75.f, 75.f };
	//max = mPosition + Vector2{ 75.f, 75.f };
	//MyRectangle attack_collider{ min, max };

	//mpWorld->add_actor(new AttackCollider{ mpWorld,attack_collider, mTag, 15.f , 10.f });
}

void Attacker::move(float delta_time)
{
	Vector2 velocity = mVelocity * delta_time;
	mPosition += velocity;
	mCollider = mCollider.translate(velocity);
}