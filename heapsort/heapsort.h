/* heap sort

by Atlee Brink
version 2008.02.12

About

	For sorting a list by way of a custom comparison function.
	The list can contain any data, but it must be the same size as a pointer.
	Does not recurse.
	Does not allocate any memory.
	Worst case should be approximately O(n log n).
	Best case isn't much better than worst case.
	Not stable.

*/

#pragma once

void heapsort(void *_list, unsigned int size, int (compare)(void*, void*));