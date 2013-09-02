/* smoothsort

by Atlee Brink, based on manuscript EWD796a.
version 2008.02.12

About

	Not as fast as claimed. Number of operations is significantly more than
	heapsort both in the worst case and in the average case. Sorted or nearly
	sorted data is, however, much faster than heapsort.
	
	The number of comparisons is much higher than heapsort, and for non sequential
	data, the number of swaps is also much higher.
	
	I have not identified a specific worst case, however random data gives
	the worst results out of sequential, random, and reverse sequential.
	
	Sort time is approximately 160% of heapsort in the worst case, and perhaps
	10% in the best case.
	
*/

#pragma once

void smoothsort(void *_list, unsigned int size, int (compare)(void*, void*));
