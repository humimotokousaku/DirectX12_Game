#pragma once
#include <chrono>

class GameTimer
{
public:
    static GameTimer* GetInstance();

    GameTimer()
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
        m_LastTime = m_StartTime;
        m_DeltaTime = 0.0;
    }

    void Reset()
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
        m_LastTime = m_StartTime;
        m_DeltaTime = 0.0;
    }

    void Tick()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta = currentTime - m_LastTime;
        m_DeltaTime = delta.count();
        m_LastTime = currentTime;
    }

    double GetDeltaTime() const
    {
        return m_DeltaTime * coefficient;
    }

    double GetTotalTime() const
    {
        std::chrono::duration<double> totalTime = std::chrono::high_resolution_clock::now() - m_StartTime;
        return totalTime.count();
    }

    /// <summary>
    /// ゲーム速度を設定(1がデフォルト)
    /// </summary>
    /// <param name="gameSpeed">時間の速さ</param>
    void SetDeltaTimeMultiply(float gameSpeed) { coefficient = gameSpeed * 60.0f; };

private:
    std::chrono::high_resolution_clock::time_point m_StartTime;
    std::chrono::high_resolution_clock::time_point m_LastTime;
    double m_DeltaTime;
    float coefficient = 60.0f;
};