#include "stdafx.h"

#include "AudioOutputDevice.hpp"

AudioOutputDevice::AudioOutputDevice() :
	audioSink(NULL),
	audioBytesWritten(0),
	audioData(AUDIO_OUTPUT_BUFFER_COUNT * AUDIO_OUTPUT_BUFFER_SIZE) {
	AudioOutputDevice(L"");
}

AudioOutputDevice::AudioOutputDevice(std::wstring inputSource) :
	audioSink(NULL),
	audioBytesWritten(0),
	audioData(AUDIO_OUTPUT_BUFFER_COUNT * AUDIO_OUTPUT_BUFFER_SIZE) {
	unsigned int deviceId = deviceIdFromString(inputSource);
	openAudioOutputSink(deviceId);
	initializeAudioBlocks();
}

AudioOutputDevice::~AudioOutputDevice() {
	destroyAudioBlocks();
	closeAudioOutputSink();
}

std::vector<std::wstring> AudioOutputDevice::listAvailableSinks() {
	std::vector<std::wstring> sinkList;
	UINT sourceCount = waveOutGetNumDevs();

	for(unsigned int i = 0; i < sourceCount; i++) {
		WAVEOUTCAPS woc;
		MMRESULT mmr = waveOutGetDevCaps(i, &woc, sizeof(woc));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSinkException(errorText);
		}
		/* TODO: support different formats */
		if(WAVE_FORMAT_4M16 & woc.dwFormats) {
			sinkList.push_back(std::wstring(woc.szPname));
		}
	}

	return sinkList;
}

void AudioOutputDevice::writeAudio(const unsigned char* rawAudio, unsigned rawAudioSize) {
	if(keepPlaying == false) {
		return;
	}

	audioData.write((const char*) rawAudio, rawAudioSize);
	int lastBlock = audioBytesWritten / AUDIO_OUTPUT_BUFFER_SIZE;
	audioBytesWritten += rawAudioSize;
	int currentBlock = audioBytesWritten / AUDIO_OUTPUT_BUFFER_SIZE;

	for(int i = lastBlock; i < currentBlock; i++) {
		LPWAVEHDR waveHeader = audioBlocks + i % AUDIO_OUTPUT_BUFFER_COUNT;
		MMRESULT mmr = waveOutWrite(audioSink, waveHeader, sizeof(WAVEHDR));
		if(mmr == WAVERR_STILLPLAYING) {
			/* TODO: invent some waiting mechanism */
		} else if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSinkException(errorText);
		}
	}
}

unsigned int AudioOutputDevice::deviceIdFromString(std::wstring inputSource) {
	UINT sourceCount = waveOutGetNumDevs();
	UINT deviceId = WAVE_MAPPER;

	for(unsigned int i = 0; i < sourceCount; i++) {
		WAVEOUTCAPS woc;
		MMRESULT mmr = waveOutGetDevCaps(i, &woc, sizeof(woc));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSinkException(errorText);
		}
		if(inputSource.compare(woc.szPname) == 0) {
			deviceId = i;
		}
	}
	return deviceId;
}

void AudioOutputDevice::openAudioOutputSink(unsigned int deviceId) {
	/* TODO: support multiple formats */
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.nSamplesPerSec = 44100;
	format.wBitsPerSample = 16;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;
	format.cbSize = 0;

	MMRESULT mmr = waveOutOpen(&audioSink, deviceId, &format, (DWORD_PTR) NULL, (DWORD_PTR) this, CALLBACK_NULL);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSinkException(errorText);
	}
}

void AudioOutputDevice::closeAudioOutputSink() {
	if(audioSink == NULL) {
		return;
	}

	MMRESULT mmr = waveOutClose(audioSink);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSinkException(errorText);
	}

	audioSink = NULL;
}

void AudioOutputDevice::initializeAudioBlocks() {
	for(int i = 0; i < AUDIO_OUTPUT_BUFFER_COUNT; i++) {
		LPWAVEHDR waveHeader = audioBlocks + i;
		waveHeader->dwBufferLength = AUDIO_OUTPUT_BUFFER_SIZE;
		waveHeader->lpData = audioData + i * AUDIO_OUTPUT_BUFFER_SIZE;
		waveHeader->dwFlags = 0;

		MMRESULT mmr = waveOutPrepareHeader(audioSink, waveHeader, sizeof(WAVEHDR));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSinkException(errorText);
		}
	}
	keepPlaying = true;
}

void AudioOutputDevice::destroyAudioBlocks() {
	keepPlaying = false;

	MMRESULT mmr = waveOutReset(audioSink);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSinkException(errorText);
	}

	for(int i = 0; i < AUDIO_OUTPUT_BUFFER_COUNT; i++) {
		LPWAVEHDR waveHeader = audioBlocks + i;
		MMRESULT mmr = waveOutUnprepareHeader(audioSink, waveHeader, sizeof(WAVEHDR));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveOutGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSinkException(errorText);
		}
	}
}