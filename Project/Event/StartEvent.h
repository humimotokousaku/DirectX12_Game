#pragma once
#include "Player.h"
#include "FollowCamera.h"

/// <summary>
/// ステージ開始演出
/// </summary>
class StartEvent {
public:
	StartEvent() = default;
	~StartEvent() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="player">自機のアドレス</param>
	/// <param name="followCamera">追従カメラのアドレス</param>
	void Initialize(Player* player, FollowCamera* followCamera);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

#pragma region Getter
	/// <summary>
	/// 開始演出を行っているかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsActive() { return isActive_; }
#pragma endregion

#pragma region Setter

#pragma endregion

private:
	Player* player_;
	FollowCamera* followCamera_;

	// ステージ開始時のアニメーション
	std::array<Animation, 2> startAngleAnim_;

	WorldTransform startWorldTransform_;

	Vector3 cameraOffset_;

	bool isActive_;
};