#include "stdafx.h"

#include "AudioInputDevice.hpp"

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if(uMsg == WIM_OPEN) {
		return;
	} else if(uMsg == WIM_DATA) {
		reinterpret_cast<AudioInputDevice*>(dwInstance)->onAudioData((void*)dwParam1);
		return;
	} else if(uMsg == WIM_CLOSE) {
		return;
	}
}

AudioInputDevice::AudioInputDevice() : audioSource(NULL), audioSink(NULL) {
	AudioInputDevice(L"");
}

AudioInputDevice::AudioInputDevice(std::wstring inputSource) : audioSource(NULL), audioSink(NULL) {
	unsigned int deviceId = deviceIdFromString(inputSource);
	openAudioInputSource(deviceId);
	initializeBuffers();
	startRecording();
}

AudioInputDevice::~AudioInputDevice() {
	stopRecording();
	destroyBuffers();
	closeAudioInputSource();

}

std::vector<std::wstring> AudioInputDevice::listAvailableSources() {
	std::vector<std::wstring> sourceList;
	UINT sourceCount = waveInGetNumDevs();

	for(unsigned int i = 0; i < sourceCount; i++) {
		WAVEINCAPS wic;
		MMRESULT mmr = waveInGetDevCaps(i, &wic, sizeof(wic));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSourceException(errorText);
		}
		/* TODO: support different formats */
		if(WAVE_FORMAT_4M16 & wic.dwFormats) {
			sourceList.push_back(std::wstring(wic.szPname));
		}
	}

	return sourceList;
}

void AudioInputDevice::setAudioSink(AudioSink* audioSink) {
	this->audioSink = audioSink;
}

unsigned int AudioInputDevice::deviceIdFromString(std::wstring inputSource) {
	UINT sourceCount = waveInGetNumDevs();
	UINT deviceId = WAVE_MAPPER;

	for(unsigned int i = 0; i < sourceCount; i++) {
		WAVEINCAPS wic;
		MMRESULT mmr = waveInGetDevCaps(i, &wic, sizeof(wic));
		if(inputSource.compare(wic.szPname) == 0) {
			deviceId = i;
		}
	}
	return deviceId;
}

void AudioInputDevice::openAudioInputSource(unsigned int deviceId) {
	/* TODO: support multiple formats */
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.nSamplesPerSec = 44100;
	format.wBitsPerSample = 16;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;
	format.cbSize = 0;

	MMRESULT mmr = waveInOpen(&audioSource, deviceId, &format, (DWORD_PTR) waveInProc, (DWORD_PTR) this, CALLBACK_FUNCTION);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSourceException(errorText);
	}
}

void AudioInputDevice::closeAudioInputSource() {
	if(audioSource == NULL) {
		return;
	}
	MMRESULT mmr = waveInClose(audioSource);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSourceException(errorText);
	}

	audioSource = NULL;
}

void AudioInputDevice::initializeBuffers() {
	for(int i = 0; i < AUDIO_INPUT_BUFFER_COUNT; i++) {
		LPWAVEHDR audioBuffer = &audioBuffers[i];
		audioBuffer->dwBufferLength = AUDIO_INPUT_BUFFER_SIZE;
		audioBuffer->lpData = new char[audioBuffer->dwBufferLength];
		audioBuffer->dwFlags = 0;

		MMRESULT mmr = waveInPrepareHeader(audioSource, audioBuffer, sizeof(WAVEHDR));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSourceException(errorText);
		}

		mmr = waveInAddBuffer(audioSource, audioBuffer, sizeof(WAVEHDR));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSourceException(errorText);
		}
	}
}

void AudioInputDevice::destroyBuffers() {
	for(int i = 0; i < AUDIO_INPUT_BUFFER_COUNT; i++) {
		LPWAVEHDR audioBuffer = &audioBuffers[i];
		LPSTR data = audioBuffer->lpData;
		
		MMRESULT mmr = waveInUnprepareHeader(audioSource, audioBuffer, sizeof(WAVEHDR));
		if(mmr) {
			CHAR errorText[MAXERRORLENGTH];
			waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
			throw AudioSourceException(errorText);
		}
		delete[] data;
	}
}

void AudioInputDevice::startRecording() {
	keepRecording = true;
	MMRESULT mmr = waveInStart(audioSource);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSourceException(errorText);
	}
}

void AudioInputDevice::stopRecording() {
	keepRecording = false;
	MMRESULT mmr = waveInStop(audioSource);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSourceException(errorText);
	}

	mmr = waveInReset(audioSource);
	if(mmr) {
		CHAR errorText[MAXERRORLENGTH];
		waveInGetErrorTextA(mmr, errorText, MAXERRORLENGTH);
		throw AudioSourceException(errorText);
	}
}

void AudioInputDevice::onAudioData(void* data) {
	/* TODO: don't do this from a system thread */
	LPWAVEHDR waveHeader = reinterpret_cast<LPWAVEHDR>(data);
	if(audioSink) {
		audioSink->writeAudio((unsigned char*) waveHeader->lpData, waveHeader->dwBytesRecorded);
	}
	if(keepRecording) {
		waveInAddBuffer(audioSource, waveHeader, sizeof(WAVEHDR));
	}
}
