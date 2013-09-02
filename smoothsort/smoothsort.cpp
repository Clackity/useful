#include "smoothsort.h"

#include <stdio.h>
#include <conio.h>

#define P64 // until I figure a way around the way 'p' is manipulated, it needs to be an unsigned __int64 for arrays larger than 7049156 elements.

namespace {
	unsigned int LP[46] = {
		1, 1, 3, 5, 9, 15, 25, 41, 67, 109,
		177, 287, 465, 753, 1219, 1973, 3193, 5167, 8361, 13529,
		21891, 35421, 57313, 92735, 150049, 242785, 392835, 635621, 1028457, 1664079,
		2692537, 4356617, 7049155, 11405773, 18454929, 29860703, 48315633, 78176337, 126491971, 204668309,
		331160281, 535828591, 866988873, 1402817465, 2269806339, 3672623805
	};

	unsigned int level, level1; // to replace b, c, b1 and c1
	unsigned int r, r1;
	#ifdef P64
		unsigned __int64 p;
	#else
		unsigned int p;
	#endif

	void swap(void **a, void **b) {
		void *t = *a;
		*a = *b;
		*b = t;
	}
	
	void sift(void **m, int (compare)(void*, void*)) { // critical
		while (level1 > 1) {
			unsigned int r2 = r1 - LP[level1] + LP[level1 - 1];
			if (compare(m[r1 - 1], m[r2]) > 0) { // 2nd critical
				r2 = r1 - 1;
				level1--;
			}
			if (compare(m[r1], m[r2]) < 0) { // 1st critical
				swap(m + r1, m + r2);
				r1 = r2;
				level1--;
			} else { // 3rd critical
				level1 = 1;
			}
		}
	}

	void trinkle(void **m, int (compare)(void*, void*)) {
		#ifdef P64
			unsigned __int64 p1 = p;
		#else
			unsigned int p1 = p;
		#endif
		level1 = level;
		while (p1) {
			while (!(p1 & 1)) {
				p1 >>= 1;
				level1++;
			}
			unsigned int r3 = r1 - LP[level1];
			if (p1 == 1 || compare(m[r3], m[r1]) <= 0) p1 = 0;
			else {
				p1--;
				if (level1 < 2) {
					swap(m + r1, m + r3);
					r1 = r3;
				} else {
					unsigned int r2 = r1 - LP[level1] + LP[level1 - 1];
					if (compare(m[r2], m[r1 - 1]) < 0) {
						r2 = r1 - 1;
						level1--;
						p1 <<= 1;
					}
					if (compare(m[r3], m[r2]) > 0) {
						swap(m + r1, m + r3);
						r1 = r3;
					} else {
						swap(m + r1, m + r2);
						r1 = r2;
						level1--;
						p1 = 0;
					}
				}
			}
		}
		sift(m, compare);
	}

	void semitrinkle(void **m, int (compare)(void*, void*)) {
		r1 = r - LP[level - 1];
		if (compare(m[r1], m[r]) > 0) {
			swap(m + r, m + r1);
			trinkle(m, compare);
		}
	}
}

void smoothsort(void *_list, unsigned int size, int (compare)(void*, void*)) {
	unsigned int q = 1;
	r = 0;
	p = 1; level = 1;
	for (; q < size; q++) {
		r1 = r;
		if ((p & 7) == 3) {
			level1 = level;
			sift((void**)_list, compare);
			p = (p + 1) >> 2;
			level += 2;
		} else if ((p & 3) == 1) {
			if (q + LP[level - 1] < size) {
				level1 = level;
				sift((void**)_list, compare);
			} else trinkle((void**)_list, compare);
			level--;
			if (level > 1) {
				p = (p << level) + 1;
				level = 1;
			} else p = (p << 1) + 1;
		}
		r++;
	}
	r1 = r;
	trinkle((void**)_list, compare);
	while (q-- != 1) {
		p--;
		if (level < 2) {
			r--;
			while (!(p & 1)) {
				p >>= 1;
				level++;
			}
		} else {
			r -= LP[level] - LP[level - 1];
			if (p > 0) semitrinkle((void**)_list, compare);
			level--;
			p = (p << 1) + 1;
			r += LP[level - 1];
			semitrinkle((void**)_list, compare);
			level--;
			p = (p << 1) + 1;
		}
	}
}
