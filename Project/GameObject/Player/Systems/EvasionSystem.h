#pragma once
#include "PlayerConfig.h"
#include "Animation.h"
#include "Object3D.h"
#include "PostEffectManager.h"
#include "GameTime.h"
#include "JustEvasionSystem.h"
#include "Particles.h"
#include "GlobalVariables.h"

class Player;
/// <summary>
/// 自機の回避システム
/// </summary>
class EvasionSystem {
public:
	EvasionSystem() = default;
	~EvasionSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="player">自機のアドレス</param>
	/// <param name="camera">カメラのアドレス</param>
	/// <param name="model">モデル</param>
	/// <param name="parent">残像オブジェクトの親</param>
	void Initialize(Player* player, Camera* camera, Model* model);
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="rotateY">自機のy軸</param>
	/// <param name="rotateX">自機のy軸</param>
	/// <param name="moveVel">自機の移動ベクトル</param>
	void Update(float rotateY, float rotateX, Vector3& moveVel);
	/// <summary>
	/// 3Dオブジェクトの描画
	/// </summary>
	void Draw();
	/// <summary>
	/// UIの描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// ジャスト回避の判定を更新
	/// </summary>
	void JustColliderUpdate();

private:
	/// <summary>
	/// ジャスト回避時の更新処理
	/// </summary>
	void JustEvasion();
	/// <summary>
	/// ジャスト回避の猶予中かを判定
	/// </summary>
	/// <returns></returns>
	bool IsJustEvasionFrame();
	/// <summary>
	/// ジャスト回避の衝突応答
	/// </summary>
	/// <param name="collider"></param>
	void JustOnCollision(Collider* collider);

	/// <summary>
	/// jsonファイルからジャスト回避時のパーティクル情報を読み込む
	/// </summary>
	void LoadJustParticleData();
	/// <summary>
	/// ジャスト回避時のパーティクルの更新処理
	/// </summary>
	/// <param name="particle"></param>
	void JustParticleUpdate(Particle& particle);

	/// <summary>
	/// ジャスト回避の無敵状態の更新
	/// </summary>
	void InvisibleUpdate();

public:
	/// <summary>
	/// 回避情報の初期化
	/// </summary>
	void Reset();

#pragma region Setter
	/// <summary>
	/// 回避行動を行うときに設定する情報
	/// </summary>
	void SetJustParameter();

	/// <summary>
	/// 残像オブジェクトの親を設定
	/// </summary>
	/// <param name="parent">親のアドレス</param>
	void SetParent(const WorldTransform* parent){
		for (int i = 0; i < afterImageObject3d_.size(); i++) {
			afterImageObject3d_[i]->worldTransform.parent_ = parent;
		}
	}

	/// <summary>
	/// ジャスト回避の判定のとるかを設定
	/// </summary>
	/// <param name="isActive"></param>
	void SetIsActiveJustCollider(const bool& isActive) { justEvasionCollider_->SetIsActive(isActive); }
#pragma endregion

#pragma region Getter
	// 回避時の情報を取得
	const EvasionData& GetEvasionData() { return evasion_; }
	// 回避中かを取得
	const bool& GetIsEvasion() { return evasion_.isActive; }
	// 回避補無敵中か
	const bool& GetIsInvisible() { return isInvinsible_; }

	/// <summary>
	/// ジャスト回避のチュートリアル中かを取得
	/// </summary>
	/// <returns></returns>
	bool GetFirstJustEvasion() {
		if (firstJustState_ == kFirstJust) { return true; }

		return false;
	}
	int GetFirstJustEvasionState() {
		if (firstJustState_ == kNone) { return 0; }
		else if (firstJustState_ == kFirstJust) { return 1; }
		return 2;
	}
#pragma endregion

private:// 定数
	// 残像オブジェクトのサイズ
	const Vector3 kAfterImageSize = { 0.5f,0.5f,0.5f };

	// ジャスト回避のアシストのために遅くする時間の値
	const float kJustAssistTimeScale = 0.6f;

	// ジャストパーティクルのjsonファイルの名前
	const std::string kJustParticleGroupName = "Player_JustParticle";

private:// エンジン機能
	// テクスチャマネージャ
	TextureManager* textureManager_;

	// jsonファイルから読み込む
	GlobalVariables* globalVariables_;

private:
	// 自機のアドレスを取得
	Player* player_;
	// ゲームタイマー
	GameTimer* gameTimer_;

	// 自機の残像
	std::array<std::unique_ptr<Object3D>, 20> afterImageObject3d_;

	// ジャスト回避の演出
	std::unique_ptr<JustEvasionSystem> justEvasionSystem_;

	// ジャスト回避時のUI
	std::unique_ptr<Sprite> justSprite_;
	// 初めてジャスト回避するときのUI
	std::unique_ptr<Sprite> firstJustSprite_;

	// ジャスト回避時のパーティクル
	std::unique_ptr<Particles> justEvasionParticle_;

	// ジャスト回避の判定
	std::unique_ptr<Collider> justEvasionCollider_;

	// 回避時の情報
	EvasionData evasion_{};

	// 回避時の移動速度のアニメーション(イージングとして使用)
	Animation evasionSpeedAnim_;
	// 回避時の回転速度のアニメーション(イージングとして使用)
	Animation evasionRotSpeedAnim_;
	// 残像のα値のアニメーション
	std::vector<Animation> evasionAlphaAnims_;
	// ジャスト回避のチュートリアル時のAボタン拡縮アニメーション
	Animation buttonScalingAnim_;

	// 残像モデルのテクスチャ
	uint32_t afterImageTexture_;
	ParticleTextures justParticleTextures_;

	// 無敵時間
	float invinsibleFrame_;

	// ジャスト回避チュートリアルの状態
	int firstJustState_ = kNone;

	// 無敵中か
	bool isInvinsible_ = false;
};