#pragma once
#include "IObject3D.h"

class Object3D : public IObject3D {
public:
	// コンストラクタ
	Object3D() = default;
	// デストラクタ
	~Object3D() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="viewProjection">カメラ</param>
	/// <param name="textureNum">テクスチャ番号</param>
	void Draw(uint32_t textureNum, int fillMode = kFillModeSolid) override;
	/// <summary>
	/// 描画処理(モデルで使われているテクスチャを使用する)
	/// </summary>
	void Draw(int fillMode = kFillModeSolid) override;

	/// <summary>
	/// ImGuiを表示
	/// </summary>
	/// <param name="name">タイトルの名前</param>
	void ImGuiParameter(const char* name);

private:
	/// <summary>
	/// 今使用しているアニメーションの検出と更新
	/// </summary>
	void CurrentAnimUpdate();

private:// プライベートな変数

};