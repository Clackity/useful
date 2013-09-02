#ifndef PCMIN_H
#define PCMIN_H


/* interface specs:

	> int pcminStart(unsigned long rate, unsigned long channels, float *buffer, unsigned long bufferlen)
			unsigned long rate [in]: desired number of samples per second
			unsigned long channels [in]: desired number of channels (1 = mono, 2 = stereo, etc.)
			float *buffer [in]: pointer to a circular buffer that must be at least [channels * bufferlen]
			unsigned long bufferlen [in]: size of the buffer in blocks
			float latency [in]: maximum desired acceptable latency, in seconds (conservative would be 1 / 50, responsive would be 1 / 500)
			unsigned long *latencyBlocks [*out]: actual latency, in blocks
			returned int:
				0 = success
				1 = unsupported format

	> unsigned long pcminLock(unsigned long *blockindex)
			unsigned long *blockindex [out]: will receive the index of the oldest unread block
			returned unsigned long: number of new blocks, if any
			
			To check the number of new blocks, call with *blockindex at 0.
			This will not incur a lock, and does not need to be followed by pcminUnlock.

	> void pcminUnlock(unsigned long blocksread)
			unsigned long blocksread [in]: the number of blocks read, starting from *blockindex

	> void pcminStop()

*/


int pcminStart(unsigned long rate, unsigned long channels, float *buffer, unsigned long bufferlen, float latency, unsigned long *latencyBlocks);
unsigned long pcminLock(unsigned long *blockindex);
void pcminUnlock(unsigned long blocksread);
void pcminStop();


#endif