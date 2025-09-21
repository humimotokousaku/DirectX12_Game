#pragma once
#include "MathStructs.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include <vector>

// トレイルの情報
struct TrailData {
	Vector3 head;
	Vector3 tail;
	bool isUsed;
};

// トレイルのバッファ
struct TrailBuffer {
	Vector3 pos;
	Vector2 uv;
};

/// <summary>
/// トレイルを表示する
/// </summary>
class TrailObject {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="bufferSize">頂点のサイズ</param>
	/// <param name="camera">カメラのアドレス</param>
	TrailObject(int bufferSize, Camera* camera);
	~TrailObject() = default;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	/// <summary>
	/// 動的に頂点を作る
	/// </summary>
	/// <param name="usedPosArray"></param>
	/// <param name="split">頂点の分割数</param>
	void CreateCurveVertex(std::vector<TrailData>& usedPosArray, int split);

	/// <summary>
	/// 頂点のメモリ確保
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// マテリアルのメモリ確保
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// 
	/// </summary>
	void CreateIndexResource();

public:
#pragma region Setter
	/// <summary>
	/// 現在のフレームのトレイル情報を保存
	/// </summary>
	/// <param name="head"></param>
	/// <param name="tail"></param>
	void SetPos(const Vector3& head, const Vector3& tail);

	/// <summary>
	/// テクスチャの番号を設定
	/// </summary>
	/// <param name="textureNum">テクスチャ番号</param>
	void SetTextureNum(const int& textureNum) { textureNum_ = textureNum; }
#pragma endregion

#pragma region Getter
	/// <summary>
	/// 現在のトレイル情報
	/// </summary>
	/// <returns></returns>
	std::vector<TrailData> GetUsedPosArray();
#pragma endregion

private:// エンジン機能
	// DirectX
	DirectXCommon* dxCommon_;
	// テクスチャマネージャ
	TextureManager* texManager_;

private:// プライベートなメンバ変数
	// トレイルの情報
	std::vector<TrailData> posArray_;
	// 頂点バッファ
	std::vector<TrailBuffer> vertex_;
	// 現在のフレームでのトレイル情報
	TrailData currentPos_;

	// マテリアル
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	// インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	// テクスチャ番号
	uint32_t textureNum_ = 1;

	// カメラ
	Camera* camera_;

	// トレイルの分割数
	int split_;
};