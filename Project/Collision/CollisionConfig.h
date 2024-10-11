#pragma once
#include "Vector3.h"
#include <cstdint>

/// 衝突属性
// プレイヤー陣営
const uint32_t kCollisionAttributePlayer = 0b1;
// 敵陣営
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;
// 障害物
const uint32_t kCollisionAttributeObstacles = 0b1 << 2;

/// 当たり判定の形状
// 球体
const uint32_t kCollisionSphere = 0b10;
// AABB
const uint32_t kCollisionAABB = 0b10 << 1;
// OBB
const uint32_t kCollisionOBB = 0b10 << 2;

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