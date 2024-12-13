#pragma once
#include <chrono>

/// <summary>
/// 時間の速さを管理するクラス
/// </summary>
class GameTimer {
public:
    // シングルトン
    static GameTimer* GetInstance();

    // コンストラクタ
    GameTimer() = default;
    // デストラクタ
    ~GameTimer() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

#pragma region Setter
    // 時間の速さを設定
    void SetTimeScale(float timeScale) { timeScale_ = timeScale; }
#pragma endregion

#pragma region Getter
    // 時間の速さを取得
    float GetTimeScale() { return timeScale_; }
#pragma endregion

private:
    // 時間の速さ[1がデフォ]
    float timeScale_ = 1.0f;
};