/* template binary heap sort

by Atlee Brink
version 2008.02.16

Usage

	void theapsort<typename T, int compare(T, T)>(T *list, unsigned int size);

	'compare' should return <0, 0, or >0 as per usual logical comparison rules.

About

	Ascending, unstable sort.
	Use tqheapsort for arrays larger than about 1,000,000 elements.

*/

#pragma once

namespace theapsort_h {
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
			unsigned int b = (a << 1) + 1, c = b + 1;
			if (b >= heap) break;
			if (compare(list[a], list[b]) < 0) {
				if (c < heap && compare(list[b], list[c]) < 0) swapr(list, a, c);
				else swapr(list, a, b);
			} else if (c < heap && compare(list[a], list[c]) < 0) swapr(list, a, c);
			else break;
		}
	}
}

template <typename T, int compare(T, T)>
void theapsort(T *list, unsigned int size) {
	if (!list || size <= 1) return;
	unsigned int heap = 1;
	while (heap < size) {
		unsigned int d = heap++;
		do {
			unsigned int a = (d - 1) >> 1;
			if (compare(list[a], list[d]) < 0) theapsort_h::swapr<T>(list, d, a);
			else break;
		} while (d);
		theapsort_h::sink<T, compare>(list, heap, d);
	}
	while (--heap) {
		theapsort_h::swap<T>(list[0], list[heap]);
		theapsort_h::sink<T, compare>(list, heap, 0);
	}
}