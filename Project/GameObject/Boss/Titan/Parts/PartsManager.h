#pragma once
// Manager
#include "ModelManager.h"
#include "TextureManager.h"
// Parts
#include "LeftHand.h"
#include "RightHand.h"
#include "WeakPoint.h"
#include <algorithm>
#include <map>
#include <optional>

class EnemyManager;
class Player;
/// <summary>
/// タイタンのパーツを管理するクラス
/// </summary>
class PartsManager {
public:
	// 振るまい
	enum class TitanBehavior {
		Wait,			// 通常状態
		HandAttack,		// シーン遷移開始演出
		BeamAttack,		// ビーム攻撃
		RevealWeakPoint	// 弱点露出
	};

public:
	PartsManager() = default;
	~PartsManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WorldTransform* object3d, Camera* camera, EnemyManager* enemyManager, int id);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// 
	/// User Method
	/// 

private:
	// 右手の生成
	void CreateRightHand(int id);
	// 左手の生成
	void CreateLeftHand(int id);
	// 弱点の生成
	void CreateWeakPoint(int id);

#pragma region 振る舞い
	// 振る舞いの更新処理
	void BehaviorUpdate();

	// 待機状態
	void B_WaitInit();
	void B_WaitUpdate();

	// 両手攻撃
	void B_HandAttackInit();
	void B_HandAttackUpdate();

	// ビーム攻撃
	void B_BeamAttackInit();
	void B_BeamAttackUpdate();

	// 弱点露出状態
	void B_RevealWeakPointInit();
	void B_RevealWeakPointUpdate();
#pragma endregion

public:
#pragma region 振る舞い
	// パーツを待機状態にする
	void StartWait() { behaviorRequest_ = TitanBehavior::Wait; }
	// パーツを両手攻撃状態にする
	void StartHandAttack() { behaviorRequest_ = TitanBehavior::HandAttack; }
	// パーツをビーム攻撃状態にする
	void StartBeamAttack() { behaviorRequest_ = TitanBehavior::BeamAttack; }
	// パーツを弱点露出状態にする
	void StartRevealWeakPoint() { behaviorRequest_ = TitanBehavior::RevealWeakPoint; }
#pragma endregion

#pragma region Getter
	// 状態変更可能かを取得
	bool GetIsStateChangeable() { return isStateChangeable_; }
#pragma endregion

#pragma region Setter
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// 敵管理クラスのアドレスを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
#pragma endregion

private:// エンジン機能
	ModelManager* modelManager_;
	TextureManager* textureManager_;

private:
	// 使用するモデル
	std::vector<Model*> models_;

	// ボスのパーツ
	std::map<std::string, IPart*> parts_;

	// ボスの体(親子関係の親に当たるオブジェクト)
	WorldTransform* rootObject_;

	// 敵管理クラスのアドレス
	EnemyManager* enemyManager_;
	// カメラのアドレス
	Camera* camera_;
	// 自機のアドレス
	Player* player_;

	// 現在の振るまい
	TitanBehavior behavior_ = TitanBehavior::Wait;
	// 次の振るまいリクエスト
	std::optional<TitanBehavior> behaviorRequest_ = std::nullopt;

	// 状態変更可能
	bool isStateChangeable_ = true;
};