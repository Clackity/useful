#ifndef CYCLIC_H
#define CYCLIC_H


inline unsigned long cyclicDifference(unsigned long max, unsigned long newer, unsigned long older) {
	if (newer >= older) return newer - older;
	return newer + (max - older);
}


inline unsigned long cyclicAdd(unsigned long max, unsigned long current, unsigned long amount) {
	if (max - current > amount) return current + amount;
	return amount - (max - current);
}


inline unsigned long cyclicSub(unsigned long max, unsigned long current, unsigned long amount) {
	if (amount > current) return max - (amount - current);
	return current - amount;
}


#endif