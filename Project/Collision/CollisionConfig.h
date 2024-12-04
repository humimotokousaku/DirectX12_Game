#pragma once
#include "Vector3.h"
#include <cstdint>

/// 衝突属性
// プレイヤー陣営
const uint32_t kCollisionAttributePlayer = 1;
// 敵陣営
const uint32_t kCollisionAttributeEnemy = 2;
// 障害物
const uint32_t kCollisionAttributeObstacles = 3;
// ジャスト回避陣営
const uint32_t kCollisionAttributeJustEvasion = 5;

/// 当たり判定の形状
// OBB
const uint32_t kCollisionOBB = 6;

// 立方体
struct cAABB {
	Vector3 max = { 1.0f,1.0f,1.0f };
	Vector3 min = { -1.0f,-1.0f,-1.0f };
};
// OBB
struct OBB {
	Vector3 m_Pos;              // 位置
	Vector3 m_fLength;          // 各軸方向の長さ
	Vector3 m_NormaDirect[3];   // 方向ベクトル
};