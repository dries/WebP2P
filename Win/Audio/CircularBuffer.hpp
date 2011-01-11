#pragma once

class CircularBuffer {
	unsigned bufferSize;
	char* buffer;
	char* currentPosition;
public:
	CircularBuffer(unsigned size) : bufferSize(size) {
		currentPosition = buffer = new char[size];
	}
	~CircularBuffer() {
		delete[] buffer;
	}

	void write(const char* data, unsigned length) {
		unsigned bytesWritten = 0;
		while(bytesWritten != length) {
			unsigned bytesRemaining = buffer + bufferSize - currentPosition;
			if(bytesRemaining > length - bytesWritten) {
				unsigned count = length - bytesWritten;
				memcpy(currentPosition, data + bytesWritten, count);
				bytesWritten += count;
				currentPosition += count;
			} else {
				memcpy(currentPosition, data + bytesWritten, bytesRemaining);
				bytesWritten += bytesRemaining;
				currentPosition = buffer;
			}
		}
	}

	operator char*() {
		return buffer;
	}
};