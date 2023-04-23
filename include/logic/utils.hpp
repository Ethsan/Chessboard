#pragma once

// Bit Functions {{{
constexpr int clz(unsigned int integer) { return __builtin_clz(integer); }

constexpr int clz(unsigned int long integer) { return __builtin_clzl(integer); }

constexpr int clz(unsigned int long long integer) {
	return __builtin_clzll(integer);
}

constexpr int ctz(unsigned int integer) { return __builtin_ctz(integer); }

constexpr int ctz(unsigned int long integer) { return __builtin_ctzl(integer); }

constexpr int ctz(unsigned int long long integer) {
	return __builtin_ctzll(integer);
}

constexpr int popcount(unsigned int integer) {
	return __builtin_popcount(integer);
}

constexpr int popcount(unsigned int long integer) {
	return __builtin_popcountl(integer);
}

constexpr int popcount(unsigned int long long integer) {
	return __builtin_popcountll(integer);
}

template <typename T>
constexpr int msb(T integer) {
	return sizeof(integer) * 8 - 1 - clz(integer);
}

template <typename T>
constexpr int lsb(T integer) {
	return ctz(integer);
}

template <typename T>
inline int pop_lsb(T &integer) {
	const int index = lsb(integer);
	integer         = integer & (integer - 1);
	return index;
} /*}}}*/
