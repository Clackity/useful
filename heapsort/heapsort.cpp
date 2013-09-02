#include "heapsort.h"

#include <stdio.h>

namespace {
	void swap(void **a, void **b) {
		void *c = *a;
		*a = *b;
		*b = c;
	}

	void swapr(void **a, void **b, unsigned int &ia, unsigned int &ib) {
		void *c = *a;
		*a = *b;
		*b = c;
		unsigned int ic = ia;
		ia = ib;
		ib = ic;
	}

	void sink(void **list, unsigned int heap, unsigned int a, int (compare)(void*, void*)) {
		for (;;) {
			unsigned int b = (a << 1) + 1, c = b + 1;
			if (b >= heap) break;
			if (compare(list[a], list[b]) < 0) {
				if (c < heap && compare(list[b], list[c]) < 0) swapr(list + a, list + c, a, c);
				else swapr(list + a, list + b, a, b);
			} else if (c < heap && compare(list[a], list[c]) < 0) swapr(list + a, list + c, a, c);
			else break;
		}
	}
}

void heapsort(void *_list, unsigned int size, int (compare)(void*, void*)) {
	if (!_list || size <= 1) return;
	void **list = (void**)_list;
	unsigned int heap = 1;
	while (heap < size) {
		unsigned int d = heap++;
		do {
			unsigned int a = (d - 1) >> 1;
			if (compare(list[a], list[d]) < 0) swapr(list + a, list + d, a, d);
			else break;
		} while (d);
		sink(list, heap, d, compare);
	}
	while (--heap) {
		swap(list, list + heap);
		sink(list, heap, 0, compare);
	}
}