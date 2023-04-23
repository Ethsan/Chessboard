#pragma once
#include <stdio.h>

#include <cstdint>
#include <ios>

#include "utils.hpp"

#define BOARD_CLEAR (Bitboard)0x0000000000000000
#define BBLINE_1    (Bitboard)0x00000000000000FF
#define BBROW_A     (Bitboard)0x0101010101010101
#define DIAG_A18H   (Bitboard)0x8040201008040201
#define DIAG_H1A8   (Bitboard)0x0102040810204080

namespace board {

typedef uint64_t Bitboard;

// clang-format off
enum Square : int {
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
	SQ_NONE,

	SQUARE_NB = 64
};
// clang-format on

// Line {{{
enum Line : int {
	LINE_1,
	LINE_2,
	LINE_3,
	LINE_4,
	LINE_5,
	LINE_6,
	LINE_7,
	LINE_8,

	LINE_NB = 8
}; /*}}}*/

// Row {{{
enum Row : int {
	ROW_A,
	ROW_B,
	ROW_C,
	ROW_D,
	ROW_E,
	ROW_F,
	ROW_G,
	ROW_H,

	ROW_NB = 8
}; /*}}}*/

// Direction {{{
enum Direction : int {
	NORTH,       // lsb
	SOUTH,       // msb
	EAST,        // lsb
	WEST,        // msb
	NORTH_WEST,  // lsb
	SOUTH_EAST,  // msb
	NORTH_EAST,  // lsb
	SOUTH_WEST,  // msb

	DIRECTION_NB = 8
}; /*}}}*/

constexpr Bitboard bb_of(Row row) { return BBROW_A << row; };
constexpr Bitboard bb_of(Line line) { return BBLINE_1 << 8 * line; };
constexpr Bitboard bb_of(Square square) { return (Bitboard)1 << square; }

constexpr Bitboard walls[] = {
    bb_of(LINE_8),                 // NORTH
    bb_of(LINE_1),                 // SOUTH
    bb_of(ROW_H),                  // EAST
    bb_of(ROW_A),                  // WEST
    bb_of(LINE_8) | bb_of(ROW_A),  // NORTH_WEST
    bb_of(LINE_1) | bb_of(ROW_H),  // SOUTH_EAST
    bb_of(LINE_8) | bb_of(ROW_H),  // NORTH_EAST
    bb_of(LINE_1) | bb_of(ROW_A),  // SOUTH_WEST
};                                 /*}}}*/

constexpr Direction opposite_direction(Direction d) {
	return (d % 2 == 0 ? Direction(d + 1) : Direction(d - 1));
}

constexpr Direction operator!(Direction d) { return opposite_direction(d); }

constexpr Direction operator~(Direction d) { return opposite_direction(d); }

template <Row r>
constexpr bool is_on_row(Square square) {
	return square % 8 == r;
}

template <Line l>
constexpr bool is_on_line(Square square) {
	return square / 8 == l;
}

constexpr Bitboard safe_bb_of(Square square) {
	return (BBLINE_1 << square) * (SQ_A1 <= square & square <= SQ_G8);
}

constexpr Bitboard safe_bb_of(Line line) {
	return (BBLINE_1 << 8 * line) * (LINE_1 <= line & line <= LINE_8);
}

constexpr Bitboard safe_bb_of(Row row) {
	return (BBROW_A << row) * (ROW_A <= row & row <= ROW_H);
}

constexpr Row row_of(Square square) { return Row(square % 8); }
constexpr Line line_of(Square square) { return Line(square / 8); }

template <Direction d>
constexpr Square closest_collision(Bitboard ray, Bitboard obstacles) {
	if (d % 2 == 0) {
		return Square(lsb(ray & (obstacles | walls[d])));
	} else {
		return Square(msb(ray & (obstacles | walls[d])));
	}
}

template <Direction d>
constexpr Bitboard ray(Square from) {
	switch (d) {
	case NORTH:
		return BBROW_A << (row_of(from) + 8 * line_of(from));
	case SOUTH:
		return (BBROW_A << row_of(from)) >> 8 * (7 - line_of(from));
	case EAST:
		return BBLINE_1 >> row_of(from)
				       << (row_of(from) + 8 * line_of(from));
	case WEST:
		return BBLINE_1 >> (7 - row_of(from)) << 8 * line_of(from);
	case NORTH_WEST:
		return DIAG_H1A8 << 8 * (7 - row_of(from)) >>
		       8 * (7 - row_of(from)) >>
		       (7 - row_of(from)) << 8 * line_of(from);
	case SOUTH_EAST:
		return DIAG_H1A8 >>
		       8 * row_of(from) << 8 * row_of(from) << row_of(from) >>
		       8 * (7 - line_of(from));
	case NORTH_EAST:
		return DIAG_A18H << 9 * row_of(from) >>
		       8 * row_of(from) << 8 * line_of(from);
	case SOUTH_WEST:
		return DIAG_A18H >>
		       9 * (7 - row_of(from)) << 8 * (7 - row_of(from)) >>
		       8 * (7 - line_of(from));
	}
}

template <Direction d>
constexpr Bitboard ray_between(Square from, Square to) {
	return ray<d>(from) & ray<opposite_direction(d)>(to);
}

template <Direction d>
constexpr Bitboard ray_between(Square square, Bitboard obstacles) {
	const Bitboard forward  = ray<d>(Square(square));
	const Bitboard backward = ray<opposite_direction(d)>(
	    closest_collision<d>(forward, obstacles));
	return forward & backward;
}

constexpr Bitboard operator|(Bitboard bitboard, Square square) {
	return bb_of(square) | bitboard;
}

constexpr Bitboard operator|(Square square, Bitboard bitboard) {
	return bb_of(square) | bitboard;
}

constexpr Bitboard operator|(Bitboard bitboard, Row row) {
	return bb_of(row) | bitboard;
}

constexpr Bitboard operator|(Row row, Bitboard bitboard) {
	return bb_of(row) | bitboard;
}

constexpr Bitboard operator|(Bitboard bitboard, Line line) {
	return bb_of(line) | bitboard;
}

constexpr Bitboard operator|(Line line, Bitboard bitboard) {
	return bb_of(line) | bitboard;
}

constexpr Bitboard operator&(Bitboard bitboard, Square square) {
	return bb_of(square) & bitboard;
}

constexpr Bitboard operator&(Square square, Bitboard bitboard) {
	return bb_of(square) & bitboard;
}

constexpr Bitboard operator~(Square square) {
	return ~bb_of(square);
}

constexpr Bitboard operator~(Row row) {
	return ~bb_of(row);
}

constexpr Bitboard operator~(Line line) {
	return ~bb_of(line);
}

}  // namespace bboard
