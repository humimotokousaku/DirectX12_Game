#pragma once
#include "Player.h"
#include "FollowCamera.h"
#include "RailCamera.h"

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
	/// <param name="railCamera">レールカメラのアドレス</param>
	void Initialize(Player* player, FollowCamera* followCamera, RailCamera* railCamera);
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
	/// <summary>
	/// 全ての開始演出が終了したかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsEnd() { return isEnd_; }
#pragma endregion

#pragma region Setter

#pragma endregion

private:
	Player* player_;
	FollowCamera* followCamera_;
	RailCamera* railCamera_;

	// ステージ開始時のアニメーション
	std::array<Animation, 2> startAngleAnim_;

	WorldTransform startWorldTransform_;


	// 追従対象のオブジェクト
	//WorldTransform followObject_;

	Vector3 velocity_;

	Vector3 cameraOffset_;

	bool isActive_;
	bool isEnd_;
};