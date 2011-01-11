#pragma once
#pragma warning (disable: 4290)

#include "AudioSink.hpp"
#include "CircularBuffer.hpp"

class AudioSinkException : public std::exception {
	const char* str;
public:
	AudioSinkException(const char* what) : str(what){};
	
	const char* what() { return str; }
};

class AudioOutputDevice : public AudioSink {
	static const int AUDIO_OUTPUT_BUFFER_COUNT = 10;
	static const int AUDIO_OUTPUT_BUFFER_SIZE = 512;

	unsigned audioBytesWritten;

	CircularBuffer audioData;
	bool keepPlaying;

	HWAVEOUT audioSink;
	WAVEHDR  audioBlocks[AUDIO_OUTPUT_BUFFER_COUNT];
public:
	/**
	 * Creates a new audio output device. The default audio output hardware will
	 * be used.
	 */
	AudioOutputDevice();
	/**
	 * Creates a new audio output device. The parameter determines the used
	 * hardware.
	 * Use listAvailableSinks() to get a list of valid values for the
	 * parameter.
	 */
	AudioOutputDevice(std::wstring inputSource) throw(AudioSinkException);

	/**
	 * Destructor
	 */
	~AudioOutputDevice();

	/**
	 * Lists all valid audio input hardware.
	 */
	static std::vector<std::wstring> listAvailableSinks() throw(AudioSinkException);

	/**
	 * Writes raw audio to the output device.
	 */
	void writeAudio(const unsigned char* rawAudio, unsigned rawAudioSize);
private:
	unsigned int deviceIdFromString(std::wstring inputSource);

	void openAudioOutputSink(unsigned int deviceId);
	void closeAudioOutputSink();

	void initializeAudioBlocks();
	void destroyAudioBlocks();
};