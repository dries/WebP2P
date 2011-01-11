#pragma once

class AudioSink {
public:
	virtual void writeAudio(const unsigned char* rawAudio, unsigned rawAudioSize) = 0;
};