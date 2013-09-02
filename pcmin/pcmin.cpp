#include <windows.h>
#include "cyclic.h"
#include "pcmin.h"


namespace pcmin {
	const unsigned long numBuffers = 32;
	const float r8b = 1.0f / 127.5f;
	const float r16b = 1.0f / 32767.5f;
	const float r32b = 1.0f / 2147483647.5f;
	const float h16b = 32768.0f;
	const float h32b = 2147483648.0f;

	WAVEFORMATEX wfx;
	HWAVEIN hwi = 0;
	char *pbuffers = 0;
	WAVEHDR *pheaders = 0;
	unsigned long stopcount = 0;
	float *fbuffer;
	unsigned long fblen, fbwrite, fbread; // in blocks
	int locked = 0;
	struct {
		HWAVEIN hwi;
		UINT umsg;
		DWORD instance;
		DWORD param1;
		DWORD param2;
	} deferred[numBuffers];
	unsigned long numdeferred = 0;

	void CALLBACK waveInProc(HWAVEIN hwi, UINT umsg, DWORD instance, DWORD param1, DWORD param2);
	void defer(HWAVEIN hwi, UINT umsg, DWORD instance, DWORD param1, DWORD param2);
}


using namespace pcmin;


int pcminStart(unsigned long rate, unsigned long channels, float *buffer, unsigned long bufferlen, float latency, unsigned long *latencyBlocks) {
	HRESULT hr;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channels;
	wfx.nSamplesPerSec = rate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = sizeof(WAVEFORMATEX);

	unsigned long blockLatency = (unsigned long)(float(wfx.nSamplesPerSec) * latency);
	blockLatency = max(1, blockLatency);
	*latencyBlocks = blockLatency;

	hr = waveInOpen(&hwi, WAVE_MAPPER, &wfx, (DWORD)waveInProc, 0, CALLBACK_FUNCTION | WAVE_FORMAT_DIRECT);
	if (hr == MMSYSERR_NOERROR) {
		pbuffers = new char [numBuffers * blockLatency * wfx.nBlockAlign];
		if (pbuffers) {
			pheaders = new WAVEHDR [numBuffers];
			if (pheaders) {
				LPSTR pbuffer = pbuffers;
				DWORD bufferlength = blockLatency * wfx.nBlockAlign;
				WAVEHDR *whdr = pheaders;
				int i = 0;
				for (; i < numBuffers; i++, pbuffer += bufferlength, whdr++) {
					whdr->lpData = pbuffer;
					whdr->dwBufferLength = bufferlength;
					whdr->dwFlags = 0;
					whdr->dwUser = i;
					hr = waveInPrepareHeader(hwi, whdr, sizeof(WAVEHDR));
					if (hr != MMSYSERR_NOERROR) break;
					hr = waveInAddBuffer(hwi, whdr, sizeof(WAVEHDR));
					if (hr != MMSYSERR_NOERROR) {
						waveInUnprepareHeader(hwi, whdr, sizeof(WAVEHDR));
						break;
					}
				}
				if (hr == MMSYSERR_NOERROR) {
					stopcount = 0;
					fbuffer = buffer;
					fblen = bufferlen;
					fbwrite = 0;
					fbread = 0;
					locked = 0;
					numdeferred = 0;
					if (waveInStart(hwi) == MMSYSERR_NOERROR) return 0;
				} else {
					fbuffer = 0;
					fblen = 0;
					stopcount = i;
					waveInReset(hwi);
					for (; i; i--, whdr--) waveInUnprepareHeader(hwi, whdr, sizeof(WAVEHDR));
				}
				delete pheaders;
				pheaders = 0;
			}
			delete pbuffers;
			pbuffers = 0;
		}
		waveInClose(hwi);
		hwi = 0;
	} else if (hr == WAVERR_BADFORMAT) return 1;
	return -1;
}


unsigned long pcminLock(unsigned long *blockindex) {
	if (!blockindex) return cyclicDifference(fblen, fbwrite, fbread);
	if (locked) return 0;
	if (fbread == fbwrite) return 0;
	locked = 1;
	*blockindex = fbread;
	return cyclicDifference(fblen, fbwrite, fbread);
}


void pcminUnlock(unsigned long blocksread) {
	if (!locked) return;
	fbread = cyclicAdd(fblen, fbread, blocksread);
	locked = 0;
	for (unsigned long i = 0; i < numdeferred; i++) waveInProc(deferred[i].hwi, deferred[i].umsg, deferred[i].instance, deferred[i].param1, deferred[i].param2);
	numdeferred = 0;
}


void pcminStop() {
	if (!hwi) return;
	stopcount = numBuffers;
	pcminUnlock(0);
	while (stopcount) Sleep(5);
	waveInStop(hwi);
	waveInClose(hwi);
	delete pheaders;
	pheaders = 0;
	delete pbuffers;
	pbuffers = 0;
	hwi = 0;
}


void CALLBACK pcmin::waveInProc(HWAVEIN hwi, UINT umsg, unsigned long instance, unsigned long param1, unsigned long param2) {
	LPWAVEHDR pwh = (LPWAVEHDR)param1;
	if (umsg == WIM_DATA) {
		if (stopcount) {
			waveInUnprepareHeader(hwi, pwh, sizeof(WAVEHDR));
			stopcount--;
			return;
		}
		if (!fbuffer || !fblen) {
			waveInUnprepareHeader(hwi, pwh, sizeof(WAVEHDR));
			return;
		}
		unsigned long numBlocks = pwh->dwBytesRecorded / wfx.nBlockAlign;
		if (locked && cyclicDifference(fblen, fbread, fbwrite) < numBlocks) {
			defer(hwi, umsg, instance, param1, param2);
			return;
		}
		unsigned long i, maxwrite = fblen - fbwrite;
		float *dest = fbuffer + (wfx.nChannels * fbwrite);
		switch (wfx.wBitsPerSample) {
			case 8: { // unsigned byte [0..255]
				unsigned char *source = (unsigned char*)pwh->lpData;
				if (numBlocks >= maxwrite) { // 2 parts
					for (i = maxwrite * wfx.nChannels; i; i--, dest++) *dest = (r8b * float(*source++)) - 1.0f;
					dest = fbuffer;
					for (i = (numBlocks - maxwrite) * wfx.nChannels; i; i--, source++, dest++) *dest = (r8b * float(*source)) - 1.0f;
				} else { // 1 part
					for (i = numBlocks * wfx.nChannels; i; i--, source++, dest++) *dest = (r8b * float(*source)) - 1.0f;
				}
			} break;
			case 16: { // signed word [-32768..32767]
				signed short *source = (short*)pwh->lpData;
				if (numBlocks >= maxwrite) { // 2 parts
					for (i = maxwrite * wfx.nChannels; i; i--, dest++) *dest = (r16b * (float(*source++) + h16b)) - 1.0f;
					dest = fbuffer;
					for (i = (numBlocks - maxwrite) * wfx.nChannels; i; i--, source++, dest++) *dest = (r16b * (float(*source) + h16b)) - 1.0f;
				} else { // 1 part
					for (i = numBlocks * wfx.nChannels; i; i--, source++, dest++) *dest = (r16b * (float(*source) + h16b)) - 1.0f;
				}
			} break;
			case 32: { // signed double word [-2147483648..2147483647]
				signed long *source = (long*)pwh->lpData;
				if (numBlocks >= maxwrite) { // 2 parts
					for (i = maxwrite * wfx.nChannels; i; i--, dest++) *dest = (r32b * (float(*source++) + h32b)) - 1.0f;
					dest = fbuffer;
					for (i = (numBlocks - maxwrite) * wfx.nChannels; i; i--, source++, dest++) *dest = (r32b * (float(*source) + h32b)) - 1.0f;
				} else { // 1 part
					for (i = numBlocks * wfx.nChannels; i; i--, source++, dest++) *dest = (r32b * (float(*source) + h32b)) - 1.0f;
				}
			} break;
		}
		if (fbwrite != fbread && cyclicDifference(fblen, fbread, fbwrite) <= numBlocks) fbread = cyclicAdd(fblen, fbwrite, numBlocks + 1);
		fbwrite = cyclicAdd(fblen, fbwrite, numBlocks);
		waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));
	}
}


void pcmin::defer(HWAVEIN hwi, UINT umsg, DWORD instance, DWORD param1, DWORD param2) {
	if (numdeferred >= numBuffers) return;
	deferred[numdeferred].hwi = hwi;
	deferred[numdeferred].umsg = umsg;
	deferred[numdeferred].instance = instance;
	deferred[numdeferred].param1 = param1;
	deferred[numdeferred].param2 = param2;
	numdeferred++;
}