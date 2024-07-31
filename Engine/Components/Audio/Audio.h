#pragma once
#include "Vector3.h"

#define XAUDIO2_HELPER_FUNCTIONS 1
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include <vector>


// チャンクヘッダ
struct ChunkHeader {
	char id[4];	  // チャンク毎のID
	int32_t size; // チャンクサイズ
};
// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	// "RIFF"
	char type[4];		// "WAVE"
};
// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // "fmt"
	WAVEFORMATEX fmt;  // 波形フォーマット
};
// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;

	IXAudio2SourceVoice* pSourceVoice_;
	IXAudio2SubmixVoice* pSubmixVoice_;
};

class Audio
{
public:
	static Audio* GetInstance();

	// 初期化
	void Initialize();

	// 解放処理
	void Finalize();

	// 音声データの読み込み
	uint32_t SoundLoadWave(const char* filename);

	// 音声データ解放
	void SoundUnload();

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="soundData"></param>
	/// <param name="isLoop">ループ再生にするか</param>
	/// <param name="volume">音量</param>
	/// <param name="Semitones">音階</param>
	/// <param name="muffled">音のこもり具合(1がデフォルト)</param>
	void SoundPlayWave(uint32_t soundIndex, bool isLoop = false, float volume = 1.0f, float Semitones = 1.0f, float muffled = 1.0f);

	void CreateSourceVoice(uint32_t soundIndex);

private:
	IXAudio2MasteringVoice* masterVoice_;
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	std::vector<SoundData> soundData_;
	uint32_t index_;
};

