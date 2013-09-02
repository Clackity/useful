#include "qheapsort.h"
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
			unsigned int b = (a << 2) + 1;
			if (b >= heap) break;

			unsigned int e = b + 3, d = b + 2, c = b + 1;
			if (e < heap) {
				if (compare(list[a], list[b]) < 0) {
					if (compare(list[b], list[c]) < 0) {
						if (compare(list[c], list[d]) < 0) {
							if (compare(list[d], list[e]) < 0) swapr(list + a, list + e, a, e);
							else swapr(list + a, list + d, a, d);
						} else if (compare(list[c], list[e]) < 0) swapr(list + a, list + e, a, e);
						else swapr(list + a, list + c, a, c);
					} else if (compare(list[b], list[d]) < 0) {
						if (compare(list[d], list[e]) < 0) swapr(list + a, list + e, a, e);
						else swapr(list + a, list + d, a, d);
					} else if (compare(list[b], list[e]) < 0) swapr(list + a, list + e, a, e);
					else swapr(list + a, list + b, a, b);
				} else if (compare(list[a], list[c]) < 0) {
					if (compare(list[c], list[d]) < 0) {
						if (compare(list[d], list[e]) < 0) swapr(list + a, list + e, a, e);
						else swapr(list + a, list + d, a, d);
					} else if (compare(list[c], list[e]) < 0) swapr(list + a, list + e, a, e);
					else  swapr(list + a, list + c, a, c);
				} else if (compare(list[a], list[d]) < 0) {
					if (compare(list[d], list[e]) < 0) swapr(list + a, list + e, a, e);
					else swapr(list + a, list + d, a, d);
				} else if (compare(list[a], list[e]) < 0) swapr(list + a, list + e, a, e);
				else break;
			} else if (d < heap) {
				if (compare(list[a], list[b]) < 0) {
					if (compare(list[b], list[c]) < 0) {
						if (compare(list[c], list[d]) < 0) swapr(list + a, list + d, a, d);
						else swapr(list + a, list + c, a, c);
					} else if (compare(list[b], list[d]) < 0) swapr(list + a, list + d, a, d);
					else swapr(list + a, list + b, a, b);
				} else if (compare(list[a], list[c]) < 0) {
					if (compare(list[c], list[d]) < 0) swapr(list + a, list + d, a, d);
					else swapr(list + a, list + c, a, c);
				} else if (compare(list[a], list[d]) < 0) swapr(list + a, list + d, a, d);
				else break;
			} else if (c < heap) {
				if (compare(list[a], list[b]) < 0) {
					if (compare(list[b], list[c]) < 0) swapr(list + a, list + c, a, c);
					else swapr(list + a, list + b, a, b);
				} else if (compare(list[a], list[c]) < 0) swapr(list + a, list + c, a, c);
				else break;
			} else if (compare(list[a], list[b]) < 0) swapr(list + a, list + b, a, b);
			else break;
		}
	}
}

void qheapsort(void *_list, unsigned int size, int (compare)(void*, void*)) {
	if (!_list || size <= 1) return;
	void **list = (void**)_list;
	unsigned int heap = 1;
	while (heap < size) {
		unsigned int d = heap++;
		do {
			unsigned int a = (d - 1) >> 2;
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