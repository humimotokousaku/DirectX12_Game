#include "Audio.h"
#include <cassert>
#include <mfapi.h>
#include <mfidl.h>

Audio* Audio::GetInstance() {
	static Audio instance;

	return &instance;
}

void Audio::Initialize() {
	// Xaudio2エンジンのインスタンスを生成
	XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスを生成
	xAudio2_->CreateMasteringVoice(&masterVoice_);
}

void Audio::Finalize() {
	SoundUnload();
}

uint32_t Audio::SoundLoadWave(const char* filename) {
	// .wavファイルを開く
	std::ifstream file;
	std::string filePath = filename;
	filePath = "engine/resources/" + filePath;
	// .wavファイルをバイナリモードで開く
	file.open(filePath, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// .wavデータ読み込み
	// RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルはRIFF?
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプはWAVE?
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクの読み込み
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	// インデックスを更新
	uint32_t index = index_;
	index_++;
	// returnするための音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData_.push_back(soundData);
	// ソースボイスの作成
	CreateSourceVoice(index);

	return index;
}

void Audio::SoundUnload() {
	for (SoundData& soundData : soundData_) {
		soundData.pSourceVoice_->Stop(0); // 再生を停止
		soundData.pSourceVoice_->FlushSourceBuffers(); // バッファをフラッシュ
		// バッファのメモリを解放
		delete[] soundData.pBuffer;
		soundData.pBuffer = 0;
		soundData.bufferSize = 0;
		soundData.wfex = {};
	}
	soundMuffleValue = 0.0f;
	index_ = 0;
	soundData_.clear();
}

void Audio::AllSoundStop() {
	for (SoundData& soundData : soundData_) {
		soundData.pSourceVoice_->Stop(0); // 再生を停止
		soundData.pSourceVoice_->FlushSourceBuffers(); // バッファをフラッシュ
	}
}

void Audio::SoundStop(uint32_t soundIndex){ 
	soundData_[soundIndex].pSourceVoice_->Stop(0); // 再生を停止
	soundData_[soundIndex].pSourceVoice_->FlushSourceBuffers(); // バッファをフラッシュ
}

void Audio::SoundPlayWave(uint32_t soundIndex, bool isLoop, float volume, float Semitones, float muffled) {
	HRESULT result;

	// 一定の周波数以上の音を変更(今回はこもったような音になる)
	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, muffled,muffled };
	soundData_[soundIndex].pSubmixVoice_->SetFilterParameters(&FilterParameters);
	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData_[soundIndex].pBuffer;
	buf.AudioBytes = soundData_[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (isLoop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	
	// 音量の設定
	result = soundData_[soundIndex].pSourceVoice_->SetVolume(volume);
	// 音階の設定
	result = soundData_[soundIndex].pSourceVoice_->SetFrequencyRatio(Semitones);

	// 波形データの再生
	result = soundData_[soundIndex].pSourceVoice_->SubmitSourceBuffer(&buf);

	XAUDIO2_VOICE_STATE state;
	// 再生中なら音を停止して始めからやり直す
	if (soundData_[soundIndex].pSourceVoice_) {
		soundData_[soundIndex].pSourceVoice_->GetState(&state);
		if (state.BuffersQueued > 0) {
			soundData_[soundIndex].pSourceVoice_->Stop(0); // 再生を停止
			soundData_[soundIndex].pSourceVoice_->FlushSourceBuffers(); // バッファをフラッシュ
			buf.Flags = XAUDIO2_END_OF_STREAM; // バッファの終端を示すフラグを設定
			soundData_[soundIndex].pSourceVoice_->SubmitSourceBuffer(&buf); // バッファを再度サブミット
		}
	}

	// 再生開始
	result = soundData_[soundIndex].pSourceVoice_->Start();
}

void Audio::CreateSourceVoice(uint32_t soundIndex) {
	HRESULT result;
#pragma region サブミックスボイスの作成
	xAudio2_->CreateSubmixVoice(&soundData_[soundIndex].pSubmixVoice_, 2, 44100, XAUDIO2_VOICE_USEFILTER);
#pragma endregion

	// サブミックスを送る設定
	XAUDIO2_SEND_DESCRIPTOR submixSend = { 0, soundData_[soundIndex].pSubmixVoice_ };
	XAUDIO2_VOICE_SENDS submixSendList = { 1, &submixSend };

	// 波形フォーマットもとにSourceVoiceの生成
	result = xAudio2_->CreateSourceVoice(&soundData_[soundIndex].pSourceVoice_, &soundData_[soundIndex].wfex, XAUDIO2_VOICE_USEFILTER, 2.0f, 0, &submixSendList);
	assert(SUCCEEDED(result));
}
