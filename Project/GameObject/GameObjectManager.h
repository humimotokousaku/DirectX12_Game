#pragma once
#include "IObject3D.h"
#include "Player.h"

class GameObjectManager {
public:
	GameObjectManager() = default;
	~GameObjectManager() = default;

	/// <summary>
	/// シングルトン
	/// </summary>
	/// <returns></returns>
	static GameObjectManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

private:
	/// <summary>
	/// 自機の距離によってオブジェクトを非表示にする
	/// </summary>
	void CheckIsDraw();

public:
	/// <summary>
	/// ゲームオブジェクトを追加
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクトのアドレス</param>
	void AddGameObject(Object3D* gameObject) { gameObjectList_.push_back(gameObject); }

	/// <summary>
	/// 自機のアドレスを設定
	/// </summary>
	/// <param name="player">自機のアドレス</param>
	void SetPlayer(Player* player) { player_ = player; }

	void ClearGameObjectList(Object3D* gameObject) {
		gameObjectList_.remove(gameObject);
	}

private:
	// ゲームオブジェクトリスト
	std::list<IObject3D*> gameObjectList_;

	// 自機のアドレス
	Player* player_;

	float radius_ = 1000.0f;
};

