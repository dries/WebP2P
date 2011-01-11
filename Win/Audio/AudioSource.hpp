#pragma once

#include "AudioSink.hpp"

class AudioSource {
public:
	virtual void setAudioSink(AudioSink* audioSink) = 0;
};