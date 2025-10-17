#pragma once

#include <string>
#include <vector>
#include "Vector2.h"
#include "../Math/MyRectangle.h"
#include "../Math/MyTimer.h"
#include "ActorHealth.h"

class IWorld;	//ワールド抽象インターフェースの前方宣言
class BlackBoard;

//アクタークラス
class Actor {
public:
	//コンストラクタ
	Actor() = default;
	//仮想デストラクタ
	virtual ~Actor() = default;
	//更新
	virtual void update(float delta_time);
	//描画
	virtual void draw() const;
	//GUIの描画
	virtual void draw_gui();
	//メッセージ処理
	virtual void handle_message(const std::string& message, void* param);
	//死亡する
	void die();
	//死亡しているか？
	bool is_dead() const;
	//名前を取得
	const std::string& name() const;
	//タグ名を取得
	const std::string& tag() const;
	//現在位置を取得
	const Vector2& position() const;

	//攻撃力を取得
	float attack_power() const;

	//virtual void damage(const float damage_value);
	
	//コピー禁止
	Actor(const Actor& other) = delete;
	Actor& operator = (const Actor& other) = delete;

protected:
	//ワールド
	IWorld* mpWorld{ nullptr };
	//ブラックボード
	BlackBoard* mpBlackBoard{ nullptr };
	//タグ名
	std::string mTag;
	//名前
	std::string mName;
	//トランスフォーム
	Vector2 mPosition;
	//移動量
	Vector2 mVelocity{ 0.0f,0.0f };
	// 当たり判定
	MyRectangle mCollider;
	//衝突判定が有効か？
	bool mIsEnableCollider{ true };
	//死亡フラグ
	bool mIsDead{ false };
	//攻撃力
	float mAttackPower{ 0.0f };
	// 体力
	ActorHealth mHealth{ 100 };
	// ダメージクールタイム
	MyTimer mCoolTimer;
};