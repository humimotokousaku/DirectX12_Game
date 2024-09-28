#pragma once
#include "IPart.h"
#include "Animation.h"

class RightHand : public IPart {
public:
	~RightHand() override = default;
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="parent">親のワールドトランスフォーム</param>
	/// <param name="camera">カメラのアドレス</param>
	/// <param name="model">使用するモデル</param>
	/// <param name="id">管理番号</param>
	void Initialize(const WorldTransform* parent, Camera* camera, Model* model, int id) override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	// 衝突応答
	void OnCollision(Collider* collider) override;

	///
	/// Useer Method
	/// 

	/// <summary>
	/// 両手攻撃の更新処理
	/// </summary>
	void HandAttack();
	/// <summary>
	/// ビーム攻撃の更新処理
	/// </summary>
	void BeamAttack();
	/// <summary>
	/// 待機状態の更新処理
	/// </summary>
	void WaitState();
	/// <summary>
	/// 弱点露出状態の更新処理
	/// </summary>
	void RevealWeakPointState();

#pragma region Setter
	/// <summary>
	/// 当たり判定をとるかを設定
	/// </summary>
	void SetIsCollision(bool isActive) { object3d_->collider->SetIsActive(isActive); }
#pragma endregion

public:// 定数
	// 手の初期位置
	const Vector3 kDefaultPos = { 5,-3,4 };

	// 攻撃対象を狙う時間
	const int kAimFrame = 180;
	// 攻撃開始時間
	const int kAttackFrame = 240;

private:
	// 手の攻撃アニメーション
	Animation handAttackAnim_;

	// 移動ベクトル
	Vector3 velocity_;

	// 攻撃対象を狙う時間
	int aimFrame_ = 0;
};