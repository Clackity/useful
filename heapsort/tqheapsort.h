/* template quaternary heap sort

by Atlee Brink
version 2008.02.16

Usage

	void tqheapsort<typename T, int compare(T, T)>(T *list, unsigned int size);

	'compare' should return <0, 0, or >0 as per usual logical comparison rules.

About

	Ascending, unstable sort.
	Use theapsort for arrays smaller than about 1,000,000 elements.

*/

#pragma once

namespace tqheapsort_h {
	template <typename T>
	void swap(T &a, T &b) {
		T c = a;
		a = b;
		b = c;
	}

	template <typename T>
	void swapr(T *list, unsigned int &a, unsigned int b) {
		T c = list[a];
		list[a] = list[b];
		list[b] = c;
		a = b;
	}
	
	template <typename T, int compare(T, T)>
	void sink(T *list, unsigned int heap, unsigned int a) {
		for (;;) {
			unsigned int b = (a << 2) + 1;
			if (b >= heap) break;
			
			unsigned int e = b + 3, d = b + 2, c = b + 1;
			if (e < heap) {
				if (compare(list[a], list[b]) < 0) {
					if (compare(list[b], list[c]) < 0) {
						if (compare(list[c], list[d]) < 0) {
							if (compare(list[d], list[e]) < 0) swapr<T>(list, a, e);
							else swapr<T>(list, a, d);
						} else if (compare(list[c], list[e]) < 0) swapr<T>(list, a, e);
						else swapr<T>(list, a, c);
					} else if (compare(list[b], list[d]) < 0) {
						if (compare(list[d], list[e]) < 0) swapr<T>(list, a, e);
						else swapr<T>(list, a, d);
					} else if (compare(list[b], list[e]) < 0) swapr<T>(list, a, e);
					else swapr<T>(list, a, b);
				} else if (compare(list[a], list[c]) < 0) {
					if (compare(list[c], list[d]) < 0) {
						if (compare(list[d], list[e]) < 0) swapr<T>(list, a, e);
						else swapr<T>(list, a, d);
					} else if (compare(list[c], list[e]) < 0) swapr<T>(list, a, e);
					else swapr<T>(list, a, c);
				} else if (compare(list[a], list[d]) < 0) {
					if (compare(list[d], list[e]) < 0) swapr<T>(list, a, e);
					else swapr<T>(list, a, d);
				} else if (compare(list[a], list[e]) < 0) swapr<T>(list, a, e);
				else break;
			} else if (d < heap) {
				if (compare(list[a], list[b]) < 0) {
					if (compare(list[b], list[c]) < 0) {
						if (compare(list[c], list[d]) < 0) swapr<T>(list, a, d);
						else swapr<T>(list, a, c);
					} else if (compare(list[b], list[d]) < 0) swapr<T>(list, a, d);
					else swapr<T>(list, a, b);
				} else if (compare(list[a], list[c]) < 0) {
					if (compare(list[c], list[d]) < 0) swapr<T>(list, a, d);
					else swapr<T>(list, a, c);
				} else if (compare(list[a], list[d]) < 0) swapr<T>(list, a, d);
				else break;
			} else if (c < heap) {
				if (compare(list[a], list[b]) < 0) {
					if (compare(list[b], list[c]) < 0) swapr<T>(list, a, c);
					else swapr<T>(list, a, b);
				} else if (compare(list[a], list[c]) < 0) swapr<T>(list, a, c);
				else break;
			} else if (compare(list[a], list[b]) < 0) swapr<T>(list, a, b);
			else break;
		}
	}
}

template <typename T, int compare(T, T)>
void tqheapsort(T *list, unsigned int size) {
	if (!list || size <= 1) return;
	unsigned int heap = 1;
	while (heap < size) {
		unsigned int d = heap++;
		do {
			unsigned int a = (d - 1) >> 2;
			if (compare(list[a], list[d]) < 0) tqheapsort_h::swapr<T>(list, d, a);
			else break;
		} while (d);
		tqheapsort_h::sink<T, compare>(list, heap, d);
	}
	while (--heap) {
		tqheapsort_h::swap<T>(list[0], list[heap]);
		tqheapsort_h::sink<T, compare>(list, heap, 0);
	}
}