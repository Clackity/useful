/* quaternary heapsort

by Atlee Brink
version 2008.02.12

About

	Unstable.
	Does not use any additional memory.
	Very small number of swaps.
	Very good (perhaps best) worst case sort.
	Number of comparisons similar to binary heapsort.
	Better with larger arrays.
	Should perform better than binary heapsort on a CPU with better prefetch and speculative branching.

*/

#pragma once

void qheapsort(void *_list, unsigned int size, int (compare)(void*, void*));
