// vim:foldmethod=marker
#include <pthread.h>
#include <string.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>

#define BOARD_CLEAR (Bitboard)0x0000000000000000
#define BBLINE_1    (Bitboard)0x00000000000000FF
#define BBROW_A     (Bitboard)0x0101010101010101
#define DIAG_A18H   (Bitboard)0x8040201008040201
#define DIAG_H1A8   (Bitboard)0x0102040810204080

typedef uint64_t Bitboard;

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

// Enums {{{
// Square {{{
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
/*}}}*/

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

// Piece {{{
enum Piece : int {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
}; /*}}}*/

// Color {{{
enum Color {
	WHITE,
	BLACK,
	VOID,
};
/*}}}*/

// Castling {{{
enum Side {
	KINGSIDE,
	QUEENSIDE,
};

enum Castling : char {
	WHITE_KINGSIDE   = 1,
	WHITE_QUEENSIDE  = 2,
	BLACK_KINGSIDE   = 4,
	BLACK_QUEENSIDE  = 8,
	KINGSIDE_CASTLE  = WHITE_KINGSIDE | BLACK_KINGSIDE,
	QUEENSIDE_CASTLE = WHITE_QUEENSIDE | BLACK_QUEENSIDE,
	WHITE_CASTLE     = WHITE_KINGSIDE | WHITE_QUEENSIDE,
	BLACK_CASTLE     = BLACK_KINGSIDE | BLACK_QUEENSIDE,
	ALL              = WHITE_CASTLE | BLACK_CASTLE,
}; /* }}} */

// Move status
enum MoveStatus {
	UNCOMPLETE = -2,
	ILLEGAL    = -1,
	LEGAL      = 0,
	CHECK      = 1,
	PAT        = 2,
	MATE       = 3,
};

/*}}}*/

// Board {{{
//      A    B    C    D    E    F    G    H
//   .----.----.----.----.----.----.----.----.
// 8 | 56 | 57 | 58 | 59 | 60 | 61 | 62 | 63 |
//   :----+----+----+----+----+----+----+----:
// 7 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 |
//   :----+----+----+----+----+----+----+----:
// 6 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
//   :----+----+----+----+----+----+----+----:
// 5 | 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 |
//   :----+----+----+----+----+----+----+----:
// 4 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
//   :----+----+----+----+----+----+----+----:
// 3 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
//   :----+----+----+----+----+----+----+----:
// 2 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
//   :----+----+----+----+----+----+----+----:
// 1 |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
//   '----'----'----'----'----'----'----'----'
//   }}}

// Utility functions {{{
constexpr Row row_of(Square square) { return Row(square % 8); }
constexpr Line line_of(Square square) { return Line(square / 8); }

constexpr Bitboard bb_of(Row row) { return BBROW_A << row; };
constexpr Bitboard bb_of(Line line) { return BBLINE_1 << 8 * line; };
constexpr Bitboard bb_of(Square square) { return (Bitboard)1 << square; }
constexpr Color enemy(Color c) {
	return c == VOID ? VOID : c == WHITE ? BLACK : WHITE;
};

template <Row r>
constexpr bool is_on_row(Square square) {
	return square % 8 == r % 8;
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

// Wall bitboards {{{
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

template <Direction d>
inline int closest_collision(Bitboard ray, Bitboard obstacles) {
	if (d % 2 == 0) {
		return lsb(ray & (obstacles | walls[d]));
	} else {
		return msb(ray & (obstacles | walls[d]));
	}
}
constexpr Direction opposite_direction(Direction d) {
	return (d % 2 == 0 ? Direction(d + 1) : Direction(d - 1));
}

// ray {{{
template <Direction d>
constexpr Bitboard ray(Square from);

template <>
constexpr Bitboard ray<NORTH>(Square from) {
	return BBROW_A << (row_of(from) + 8 * line_of(from));
}

template <>
constexpr Bitboard ray<SOUTH>(Square from) {
	return (BBROW_A << row_of(from)) >> 8 * (7 - line_of(from));
}

template <>
constexpr Bitboard ray<EAST>(Square from) {
	return BBLINE_1 >> row_of(from) << (row_of(from) + 8 * line_of(from));
}

template <>
constexpr Bitboard ray<WEST>(Square from) {
	return BBLINE_1 >> (7 - row_of(from)) << 8 * line_of(from);
}

template <>
constexpr Bitboard ray<SOUTH_EAST>(Square from) {
	return DIAG_H1A8 >>
	       8 * row_of(from) << 8 * row_of(from) << row_of(from) >>
	       8 * (7 - line_of(from));
}

template <>
constexpr Bitboard ray<NORTH_WEST>(Square from) {
	return DIAG_H1A8 << 8 * (7 - row_of(from)) >> 8 * (7 - row_of(from)) >>
	       (7 - row_of(from)) << 8 * line_of(from);
}

template <>
constexpr Bitboard ray<SOUTH_WEST>(Square from) {
	return DIAG_A18H >> 9 * (7 - row_of(from)) << 8 * (7 - row_of(from)) >>
	       8 * (7 - line_of(from));
}

template <>
constexpr Bitboard ray<NORTH_EAST>(Square from) {
	return DIAG_A18H << 9 * row_of(from) >> 8 * row_of(from)
						    << 8 * line_of(from);
}

template <Direction d>
inline Bitboard ray_between(Square square, Bitboard obstacles) {
	const Bitboard forward  = ray<d>(Square(square));
	const Bitboard backward = ray<opposite_direction(d)>(
	    Square(closest_collision<d>(forward, obstacles)));
	return forward & backward;
}

/*}}}*/ /*}}}*/

// Print functions {{{
void bb_print(Bitboard b) {
	std::cout << "print board\n";
	for (size_t i = 0; i < 8; i++) {
		for (size_t j = 0; j < 8; j++) {
			std::cout << ((b >> (8 * (7 - i) + j)) & 1);
		}
		std::cout << std::endl;
	}
}
void bb_print(Bitboard b, std::string s) {
	std::cout << s << std::endl;
	bb_print(b);
} /*}}}*/

// Chessboard {{{
// class Chessboard {{{
class Chessboard {
	Bitboard color[2];
	Bitboard pieces[6];
	Bitboard xray_oriented[8];
	Bitboard xrays;
	Bitboard attacks;
	Bitboard check;
	Bitboard legal_moves[64];
	Bitboard virtual_pawn;
	Square enpassant;
	unsigned int turn_count;
	unsigned int legal_move_count;
	unsigned int check_count;
	Castling castling;

       public:
	Chessboard();
	int move(Square from, Square to);
	void print();
	bool isMoveLegal(Square from, Square to) const;

	inline Bitboard get_pieces() const;
	template <Piece p>
	inline Bitboard get_pieces() const;
	template <Color c>
	inline Bitboard get_pieces() const;
	template <Piece p, Color c>
	inline Bitboard get_pieces() const;

	template <Color c>
	constexpr bool can_castle() const;
	template <Color c, Side s>
	constexpr bool can_castle() const;

       private:
	void computeCoveredSquares();
	void computeLegalMoves();

	template <Color>
	inline Bitboard pawn_attack(Square square);
	template <Direction d, Color c>
	inline Bitboard ray_attack(Square square);
	template <Color>
	inline Bitboard rook_attack(Square square);
	template <Color>
	inline Bitboard bishop_attack(Square square);
	template <Color>
	inline Bitboard knight_attack(Square square);
	template <Color>
	inline Bitboard queen_attack(Square square);
	inline Bitboard king_attack(Square square);

	template <Piece p, Color c>
	inline Bitboard piece_attack(Square square);
	template <Piece p, Color c>
	inline void pieces_attack();

	template <Color>
	inline void enemy_attacks();

	template <Direction d, Color c>
	inline Bitboard ray_moves(Square square);
	template <Color>
	inline Bitboard pawn_moves(Square square);
	template <Color>
	inline Bitboard rook_moves(Square square);
	template <Color>
	inline Bitboard bishop_moves(Square square);
	template <Color>
	inline Bitboard knight_moves(Square square);
	template <Color>
	inline Bitboard queen_moves(Square square);
	template <Color>
	inline Bitboard king_moves(Square square);
	template <Piece p, Color c>
	inline Bitboard piece_moves(Square square);
	template <Piece p, Color c>
	inline void pieces_moves();
	template <Color c>
	inline void enpassant_moves();
	template <Color>
	inline void king_castle_moves();
	template <Color c, Direction d>
	inline void compute_pin();
	template <Color c>
	inline void compute_pins();

	template <Color>
	inline void compute_moves();

}; /*}}}*/

// Constructor {{{
Chessboard::Chessboard() {
	color[WHITE] = bb_of(LINE_1) | bb_of(LINE_2);
	color[BLACK] = bb_of(LINE_8) | bb_of(LINE_7);

	pieces[ROOK] =
	    bb_of(SQ_A8) | bb_of(SQ_H8) | bb_of(SQ_A1) | bb_of(SQ_H1);

	pieces[KNIGHT] =
	    bb_of(SQ_B8) | bb_of(SQ_G8) | bb_of(SQ_B1) | bb_of(SQ_G1);

	pieces[BISHOP] =
	    bb_of(SQ_C8) | bb_of(SQ_F8) | bb_of(SQ_C1) | bb_of(SQ_F1);

	pieces[KING]  = bb_of(SQ_E8) | bb_of(SQ_E1);
	pieces[QUEEN] = bb_of(SQ_D8) | bb_of(SQ_D1);

	pieces[PAWN] = bb_of(LINE_7) | bb_of(LINE_2);

	turn_count = 0;
	castling   = ALL;
}; /*}}}*/

// get_pieces {{{
inline Bitboard Chessboard::get_pieces() const {
	return color[WHITE] | color[BLACK];
}

template <Color c>
inline Bitboard Chessboard::get_pieces() const {
	return color[c];
}

template <Piece p>
inline Bitboard Chessboard::get_pieces() const {
	return pieces[p];
}

template <Piece p, Color c>
inline Bitboard Chessboard::get_pieces() const {
	return pieces[p] & color[c];
} /*}}}*/

// Can castle {{{
template <Color c>
constexpr bool Chessboard::can_castle() const {
	return castling & (c == WHITE ? WHITE_CASTLE : BLACK_CASTLE);
}

template <Color c, Side s>
constexpr bool Chessboard::can_castle() const {
	return castling & (c == WHITE ? WHITE_CASTLE : BLACK_CASTLE) &
	       (s == QUEENSIDE ? QUEENSIDE_CASTLE : KINGSIDE_CASTLE);
}
/*}}}*/

// Attack functions{{{
template <Color c>
inline Bitboard Chessboard::pawn_attack(Square square) {
	constexpr int dir = c == WHITE ? -1 : 1;

	const Bitboard pawn = bb_of(square);
	const Bitboard king = get_pieces<KING, c>();
	Bitboard attack     = bb_of(Square(square + dir * 7));

	attack |= bb_of(Square(square + dir * 9));
	attack &= bb_of(Line(line_of(square) + dir * 1));

	check |= !!(attack & king) * pawn;
	return attack;
}

template <Direction d, Color c>
inline Bitboard Chessboard::ray_attack(Square square) {
	const Bitboard piece     = bb_of(square);
	const Bitboard king      = get_pieces<KING, c>();
	const Bitboard obstacles = get_pieces() - piece;

	const Bitboard attack = ray_between<d>(square, obstacles);
	check |= !!(attack & king) * attack;
	return attack - piece;
}

template <Color c>
inline Bitboard Chessboard::rook_attack(Square square) {
	Bitboard attacks = BOARD_CLEAR;

	attacks |= ray_attack<NORTH, c>(square);
	attacks |= ray_attack<SOUTH, c>(square);
	attacks |= ray_attack<EAST, c>(square);
	attacks |= ray_attack<WEST, c>(square);

	return attacks;
}

template <Color c>
inline Bitboard Chessboard::bishop_attack(Square square) {
	Bitboard attacks = BOARD_CLEAR;

	attacks |= ray_attack<NORTH_EAST, c>(square);
	attacks |= ray_attack<NORTH_WEST, c>(square);
	attacks |= ray_attack<SOUTH_EAST, c>(square);
	attacks |= ray_attack<SOUTH_WEST, c>(square);

	return attacks;
}

template <Color c>
inline Bitboard Chessboard::queen_attack(Square square) {
	return bishop_attack<c>(square) | rook_attack<c>(square);
}

template <Piece p, Color c>
inline Bitboard Chessboard::piece_attack(Square square) {
	switch (p) {
	case PAWN:
		return pawn_attack<c>(square);
	case KNIGHT:
		return knight_attack<c>(square);
	case BISHOP:
		return bishop_attack<c>(square);
	case ROOK:
		return rook_attack<c>(square);
	case QUEEN:
		return queen_attack<c>(square);
	case KING:
		return king_attack(square);
	default:
		return BOARD_CLEAR;
	}
}

template <Piece p, Color c>
inline void Chessboard::pieces_attack() {
	const Color enemy_color = enemy(c);
	const Bitboard king     = get_pieces<KING, c>();

	Bitboard remaining = get_pieces<p, enemy_color>();
	while (remaining) {
		const Square piece       = Square(pop_lsb(remaining));
		const Bitboard bb_attack = piece_attack<p, c>(piece);
		attacks |= bb_attack;
		check_count += (bool(bb_attack & king));
	}
}

template <Color c>
inline Bitboard Chessboard::knight_attack(Square square) {
	const Bitboard king = get_pieces<KING, c>();
	const Line line     = line_of(square);
	const Row row       = row_of(square);

	const Bitboard line_inner =
	    safe_bb_of(Line(line - 1)) | safe_bb_of(Line(line + 1));
	const Bitboard line_outer =
	    safe_bb_of(Line(line - 2)) | safe_bb_of(Line(line + 2));
	const Bitboard row_inner =
	    safe_bb_of(Row(row - 1)) | safe_bb_of(Row(row + 1));
	const Bitboard row_outer =
	    safe_bb_of(Row(row - 2)) | safe_bb_of(Row(row + 2));

	const Bitboard attack =
	    (line_inner & row_outer) | (line_outer & row_inner);

	check |= bb_of(square) * !!(attack & king);
	return attack;
}

inline Bitboard Chessboard::king_attack(Square square) {
	const Line line = line_of(square);
	const Row row   = row_of(square);

	const Bitboard lines = safe_bb_of(Line(line - 1)) | bb_of(Line(line)) |
			       safe_bb_of(Line(line + 1));
	const Bitboard rows = safe_bb_of(Row(row - 1)) | bb_of(Row(row)) |
			      safe_bb_of(Row(row + 1));

	return (lines & rows) - bb_of(square);
}

template <Color c>
inline void Chessboard::enemy_attacks() {
	const Color enemy_color = enemy(c);
	attacks                 = BOARD_CLEAR;
	check                   = BOARD_CLEAR;
	check_count             = 0;

	pieces_attack<PAWN, c>();
	pieces_attack<KNIGHT, c>();
	pieces_attack<BISHOP, c>();
	pieces_attack<ROOK, c>();
	pieces_attack<QUEEN, c>();

	const Square enemy_king = Square(lsb(get_pieces<KING, enemy_color>()));
	const Bitboard bb_king_attack = king_attack(enemy_king);
	attacks |= bb_king_attack;
} /*}}}*/

// Move generation {{{
// test line 2 {{{
template <Color c>
constexpr bool isOnLine2(Square square);
template <>
constexpr bool isOnLine2<WHITE>(Square square) {
	return square < 16;
}
template <>
constexpr bool isOnLine2<BLACK>(Square square) {
	return square > 47;
} /*}}}*/

// pawn moves {{{
template <Color c>
inline Bitboard Chessboard::pawn_moves(Square square) {
	constexpr int dir = c == WHITE ? 1 : -1;
	const Line line   = line_of(square);

	Bitboard attack_moves = BOARD_CLEAR;
	attack_moves |= bb_of(Square(square + dir * 7));
	attack_moves |= bb_of(Square(square + dir * 9));
	attack_moves &= bb_of(Line(line + dir)) & get_pieces<enemy(c)>();
	Bitboard push_moves = BOARD_CLEAR;
	push_moves |= bb_of(Square(square + dir * 8));
	push_moves |= bb_of(Square(square + dir * 16)) * isOnLine2<c>(square) *
		      !(bb_of(Square(square + dir * 8)) & get_pieces());
	return attack_moves | (push_moves & ~get_pieces<c>());
} /*}}}*/

// ray moves {{{
template <Direction d, Color c>
inline Bitboard Chessboard::ray_moves(Square square) {
	const Bitboard piece     = bb_of(square);
	const Bitboard obstacles = (get_pieces() - piece) | walls[d];

	const Bitboard moves = ray_between<d>(square, obstacles) - piece;
	return moves;
} /*}}}*/

// rook moves {{{
template <Color c>
inline Bitboard Chessboard::rook_moves(Square square) {
	Bitboard moves = BOARD_CLEAR;
	moves |= ray_moves<NORTH, c>(square);
	moves |= ray_moves<SOUTH, c>(square);
	moves |= ray_moves<EAST, c>(square);
	moves |= ray_moves<WEST, c>(square);
	return moves & ~get_pieces();
} /*}}}*/

// bishop moves {{{
template <Color c>
inline Bitboard Chessboard::bishop_moves(Square square) {
	Bitboard moves = BOARD_CLEAR;
	moves |= ray_moves<NORTH_EAST, c>(square);
	moves |= ray_moves<SOUTH_EAST, c>(square);
	moves |= ray_moves<NORTH_WEST, c>(square);
	moves |= ray_moves<SOUTH_EAST, c>(square);
	return moves & ~get_pieces();
} /*}}}*/

// queen moves {{{
template <Color c>
inline Bitboard Chessboard::queen_moves(Square square) {
	return bishop_moves<c>(square) | rook_moves<c>(square);
} /*}}}*/

// knight moves {{{
template <Color c>
inline Bitboard Chessboard::knight_moves(Square square) {
	const Line line = line_of(square);
	const Row row   = row_of(square);

	const Bitboard line_inner =
	    safe_bb_of(Line(line - 1)) | safe_bb_of(Line(line + 1));
	const Bitboard line_outer =
	    safe_bb_of(Line(line - 2)) | safe_bb_of(Line(line + 2));
	const Bitboard row_inner =
	    safe_bb_of(Row(row - 1)) | safe_bb_of(Row(row + 1));
	const Bitboard row_outer =
	    safe_bb_of(Row(row - 2)) | safe_bb_of(Row(row + 2));

	return ((line_inner & row_outer) | (line_outer & row_inner)) &
	       ~get_pieces();
} /*}}}*/

// king moves {{{
template <Color c>
inline Bitboard Chessboard::king_moves(Square square) {
	const Line line = line_of(square);
	const Row row   = row_of(square);

	const Bitboard lines = safe_bb_of(Line(line - 1)) | bb_of(Line(line)) |
			       safe_bb_of(Line(line + 1));
	const Bitboard rows = safe_bb_of(Row(row - 1)) | bb_of(Row(row)) |
			      safe_bb_of(Row(row + 1));

	return (lines & rows) & ~get_pieces<c>() & ~attacks;
} /*}}}*/

// king castle {{{
template <>
inline void Chessboard::king_castle_moves<WHITE>() {
	const Bitboard bb_king_side =
	    (attacks | get_pieces()) & (bb_of(SQ_F1) | bb_of(SQ_G1));
	const Bitboard bb_queen_side =
	    (attacks | get_pieces()) &
	    (bb_of(SQ_B1) | bb_of(SQ_C1) | bb_of(SQ_D1));

	legal_moves[SQ_E1] |=
	    bb_of(SQ_G1) * !bb_king_side * can_castle<WHITE, KINGSIDE>() +
	    bb_of(SQ_C1) * !bb_queen_side * can_castle<WHITE, QUEENSIDE>();
}

template <>
inline void Chessboard::king_castle_moves<BLACK>() {
	const Bitboard bb_king_side =
	    (attacks | get_pieces()) & (bb_of(SQ_F8) | bb_of(SQ_G8));
	const Bitboard bb_queen_side =
	    (attacks | get_pieces()) &
	    (bb_of(SQ_B8) | bb_of(SQ_C8) | bb_of(SQ_D8));

	legal_moves[SQ_E8] |=
	    bb_of(SQ_G8) * !bb_king_side * can_castle<BLACK, KINGSIDE>() +
	    bb_of(SQ_C8) * !bb_queen_side * can_castle<BLACK, QUEENSIDE>();
} /*}}}*/

// en passant {{{
template <Color c>
inline void Chessboard::enpassant_moves() {
	constexpr int dir = c == WHITE ? 1 : -1;
	if (!is_on_row<ROW_A>(enpassant) &&
	    (bb_of(Square(enpassant - 1)) & get_pieces<PAWN, c>())) {
		legal_moves[enpassant - 1] |=
		    bb_of(Square(enpassant + dir * 8));
	}
	if (!is_on_row<ROW_H>(enpassant) &&
	    (bb_of(Square(enpassant + 1)) & get_pieces<PAWN, c>())) {
		legal_moves[enpassant + 1] |=
		    bb_of(Square(enpassant + dir * 8));
	}
}

// piece moves {{{
template <Piece p, Color c>
inline Bitboard Chessboard::piece_moves(Square square) {
	switch (p) {
	case PAWN:
		return pawn_moves<c>(square);
	case ROOK:
		return rook_moves<c>(square);
	case BISHOP:
		return bishop_moves<c>(square);
	case KNIGHT:
		return knight_moves<c>(square);
	case QUEEN:
		return queen_moves<c>(square);
	case KING:
		return king_moves<c>(square);
	default:
		return BOARD_CLEAR;
	}
}
/*}}}*/

// pieces moves {{{
template <Piece p, Color c>
inline void Chessboard::pieces_moves() {
	Bitboard remaining = get_pieces<p, c>();
	while (remaining) {
		const Square piece   = Square(pop_lsb(remaining));
		const Bitboard moves = piece_moves<p, c>(piece);
		legal_moves[piece]   = moves;
		legal_move_count += popcount(moves);
	}
}
/*}}}*/
template <Color c, Direction d>
inline void Chessboard::compute_pin() {
	constexpr Color enemy_c     = enemy(c);
	const Bitboard king         = get_pieces<KING, c>();
	const Bitboard ray_king     = ray<d>(Square(lsb(king)));
	const Bitboard enemy_pieces = get_pieces<enemy_c>() | walls[d];
	const Bitboard ally_pieces  = get_pieces<c>() | walls[d];
	const Square insight =
	    Square(closest_collision<d>(ray_king, enemy_pieces));
	if (d == NORTH || d == SOUTH || d == EAST || d == WEST) {
		if (!(insight & get_pieces<ROOK, enemy_c>() &
		      get_pieces<QUEEN, enemy_c>()))
			return;
	} else {
		if (!(insight & get_pieces<BISHOP, enemy_c>() &
		      get_pieces<QUEEN, enemy_c>()))
			return;
	}
	const Bitboard ray_insight = ray<opposite_direction(d)>(insight);
	const Square first_encounter =
	    Square(closest_collision<d>(ray_king, ally_pieces));
	const Square last_encounter =
	    Square(closest_collision<d>(ray_insight, ally_pieces));
	if (first_encounter == last_encounter) {
		legal_move_count -= popcount(legal_moves[first_encounter] &
					     ~(ray_insight & ray_king));
		legal_moves[first_encounter] &= ray_insight & ray_king;
	}
}

template <Color c>
inline void Chessboard::compute_pins() {
	compute_pin<c, NORTH>();
	compute_pin<c, SOUTH>();
	compute_pin<c, EAST>();
	compute_pin<c, WEST>();
	compute_pin<c, NORTH_EAST>();
	compute_pin<c, NORTH_WEST>();
	compute_pin<c, SOUTH_EAST>();
	compute_pin<c, SOUTH_WEST>();
}

// legal moves {{{
template <Color c>
inline void Chessboard::compute_moves() {
	std::fill(legal_moves, legal_moves + 64, BOARD_CLEAR);
	pieces_moves<PAWN, c>();
	pieces_moves<BISHOP, c>();
	pieces_moves<ROOK, c>();
	pieces_moves<QUEEN, c>();
	pieces_moves<KNIGHT, c>();
	pieces_moves<KING, c>();
	compute_pins<c>();
	king_castle_moves<c>();
	enpassant_moves<c>();
} /*}}}*/

/*}}}*/

/*}}}*/

// Move {{{
void Chessboard::move(Square from, Square to) {
	// TODO en passant
	Color color = Color(turn_count % 2);
}
/*}}}*/

void Chessboard::print() { /*{{{*/
	enemy_attacks<WHITE>();
	compute_moves<WHITE>();
	Bitboard move = BOARD_CLEAR;
	for (int i = 0; i < 64; i++) {
		move |= legal_moves[i];
	}
	bb_print(move);
	// print the whole board
} /*}}}*/

constexpr Bitboard rook_ray(Square square) { /*{{{*/
	return ray<SOUTH>(Square(square)) | ray<NORTH>(Square(square)) |
	       ray<EAST>(Square(square)) | ray<WEST>(Square(square));
}

constexpr Bitboard bishop_ray(Square square) {
	return ray<SOUTH_EAST>(square) | ray<SOUTH_WEST>(square) |
	       ray<NORTH_EAST>(square) | ray<NORTH_WEST>(square);
}

constexpr Bitboard fullray(Square square) {
	return rook_ray(square) | bishop_ray(square);
} /*}}}*/

int main() {
	Chessboard().print();
	return 0;
}
