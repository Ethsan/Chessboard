// vim:foldmethod=marker
#include <pthread.h>
#include <string.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <memory>
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
inline int pop_lsb(T& integer) {
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
	NONE,
	UNKNOWN = NONE,
}; /*}}}*/

// Color {{{
enum Color {
	WHITE,
	BLACK,
	VOID,
};
/*}}}*/

struct Colored_piece {
	Piece piece;
	Color color;
};

// Castling {{{
enum Side {
	KINGSIDE,
	QUEENSIDE,
};

enum Castling : char {
	WHITE_OO         = 1 << 0,
	WHITE_OOO        = 1 << 1,
	BLACK_OO         = 1 << 2,
	BLACK_OOO        = 1 << 3,
	KINGSIDE_CASTLE  = WHITE_OO | BLACK_OO,
	QUEENSIDE_CASTLE = WHITE_OOO | BLACK_OOO,
	WHITE_CASTLE     = WHITE_OO | WHITE_OOO,
	BLACK_CASTLE     = BLACK_OO | BLACK_OOO,
	ALL              = WHITE_CASTLE | BLACK_CASTLE,
}; /* }}} */

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

typedef std::array<Colored_piece, 64> Board;

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
	return r % 8 == square % 8;
}
template <Line l>
constexpr bool is_on_line(Square square) {
	return l / 8 == square / 8;
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
inline Square closest_collision(Bitboard ray, Bitboard obstacles) {
	if (d % 2 == 0) {
		return Square(lsb(ray & (obstacles | walls[d])));
	} else {
		return Square(msb(ray & (obstacles | walls[d])));
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
	Bitboard attacks;
	Bitboard check;
	Bitboard legal_moves[64];
	Square enpassant;
	unsigned int turn_count;
	unsigned int legal_move_count;
	unsigned int check_count;
	Castling castling;

       public:
	Chessboard();
	bool move(Square from, Square to, Piece promotion = UNKNOWN);
	bool isMoveLegal(Square from, Square to) const;

	inline Bitboard get_pieces() const;
	template <Piece p>
	inline Bitboard get_pieces() const;
	template <Color c>
	inline Bitboard get_pieces() const;
	template <Piece p, Color c>
	inline Bitboard get_pieces() const;
	template <Color c>
	inline bool can_castle() const;
	template <Color c, Side s>
	inline bool can_castle() const;
	inline bool is_same_as(const Chessboard& chessboard) const;
	inline bool is_attacked(Square square) const;
	inline Piece get_piece(Square square) const;
	inline Color get_color(Square square) const;
	inline Board to_board() const;

	inline Castling get_castling() const;

       private:
	inline void update_castle(Square rook);
	template <Piece p>
	inline void fill_board(Board& board) const;
	template <Color c>
	inline void fill_board(Board& board) const;
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
	constexpr Bitboard piece_attack(Square square);
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
	constexpr Bitboard piece_moves(Square square);
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
}

inline Castling Chessboard::get_castling() const { return castling; }

inline bool Chessboard::is_same_as(const Chessboard& chessboard) const {
	return get_pieces<WHITE>() == chessboard.get_pieces<WHITE>() &&
	       get_pieces<BLACK>() == chessboard.get_pieces<BLACK>() &&
	       get_pieces<PAWN>() == chessboard.get_pieces<PAWN>() &&
	       get_pieces<ROOK>() == chessboard.get_pieces<ROOK>() &&
	       get_pieces<KNIGHT>() == chessboard.get_pieces<KNIGHT>() &&
	       get_pieces<BISHOP>() == chessboard.get_pieces<BISHOP>() &&
	       get_pieces<QUEEN>() == chessboard.get_pieces<QUEEN>() &&
	       get_pieces<KING>() == chessboard.get_pieces<KING>() &&
	       turn_count % 2 == chessboard.turn_count % 2 &&
	       castling == chessboard.castling &&
	       enpassant == chessboard.enpassant;
}

inline bool Chessboard::is_attacked(Square square) const {
	return attacks & bb_of(square);
}

inline Piece Chessboard::get_piece(Square square) const {
	if (pieces[PAWN] & bb_of(square)) return PAWN;
	if (pieces[ROOK] & bb_of(square)) return ROOK;
	if (pieces[KNIGHT] & bb_of(square)) return KNIGHT;
	if (pieces[BISHOP] & bb_of(square)) return BISHOP;
	if (pieces[QUEEN] & bb_of(square)) return QUEEN;
	if (pieces[KING] & bb_of(square)) return KING;
	return NONE;
}

inline Color Chessboard::get_color(Square square) const {
	if (color[WHITE] & bb_of(square)) return WHITE;
	if (color[BLACK] & bb_of(square)) return BLACK;
	return VOID;
}

template <Color c>
inline void Chessboard::fill_board(Board& board) const {
	auto pieces = get_pieces<c>();
	while (pieces) {
		auto square         = pop_lsb(pieces);
		board[square].color = c;
	}
}

template <Piece p>
inline void Chessboard::fill_board(Board& board) const {
	auto pieces = get_pieces<p>();
	while (pieces) {
		auto square         = pop_lsb(pieces);
		board[square].piece = p;
	}
}

inline Board Chessboard::to_board() const {
	Board board            = Board();
	Colored_piece no_piece = {.piece = NONE, .color = VOID};
	std::fill(board.begin(), board.end(), no_piece);
	fill_board<PAWN>(board);
	fill_board<ROOK>(board);
	fill_board<KNIGHT>(board);
	fill_board<BISHOP>(board);
	fill_board<QUEEN>(board);
	fill_board<KING>(board);
	fill_board<WHITE>(board);
	fill_board<BLACK>(board);

	return board;
}

template <Color c>
inline bool Chessboard::can_castle() const {
	return castling & (c == WHITE ? WHITE_CASTLE : BLACK_CASTLE);
}

template <Color c, Side s>
inline bool Chessboard::can_castle() const {
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

	check |= bool(attack & king) * pawn;
	return attack;
}

template <Direction d, Color c>
inline Bitboard Chessboard::ray_attack(Square square) {
	const Bitboard piece     = bb_of(square);
	const Bitboard king      = get_pieces<KING, c>();
	const Bitboard obstacles = get_pieces() & ~piece;

	const Bitboard attack = ray_between<d>(square, obstacles);
	check |= bool(attack & king) * attack;
	return attack & ~piece;
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
constexpr Bitboard Chessboard::piece_attack(Square square) {
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
constexpr bool isOnLine2(Square square) {
	if (c == BLACK)
		return square >= 48;
	else
		return square < 16;
}

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
	Bitboard moves = BOARD_CLEAR;

	moves |= safe_bb_of(Square(square + 6));
	moves |= safe_bb_of(Square(square + 10));
	moves |= safe_bb_of(Square(square + 15));
	moves |= safe_bb_of(Square(square + 17));
	moves |= safe_bb_of(Square(square - 6));
	moves |= safe_bb_of(Square(square - 10));
	moves |= safe_bb_of(Square(square - 15));
	moves |= safe_bb_of(Square(square - 17));

	return moves & ~get_pieces();
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
template <Color c>
inline void Chessboard::king_castle_moves() {
	if (!can_castle<c>() || check_count > 0) return;

	constexpr Square king_square = c == WHITE ? SQ_E1 : SQ_E8;

	constexpr Bitboard king_side =  //
	    c == WHITE ? (bb_of(SQ_F1) | bb_of(SQ_G1))
		       : (bb_of(SQ_F8) | bb_of(SQ_G8));
	constexpr Bitboard queen_side =  //
	    c == WHITE ? (bb_of(SQ_C1) | bb_of(SQ_D1))
		       : (bb_of(SQ_C8) | bb_of(SQ_D8));

	constexpr Bitboard king_side_obstacles =
	    c == WHITE ? (bb_of(SQ_F1) | bb_of(SQ_G1))
		       : (bb_of(SQ_F8) | bb_of(SQ_G8));
	constexpr Bitboard queen_side_obstacles =
	    c == WHITE ? (bb_of(SQ_B1) | bb_of(SQ_C1) | bb_of(SQ_D1))
		       : (bb_of(SQ_B8) | bb_of(SQ_C8) | bb_of(SQ_D8));

	constexpr Bitboard king_side_castle = bb_of(c == WHITE ? SQ_G1 : SQ_G8);
	constexpr Bitboard queen_side_castle =
	    bb_of(c == WHITE ? SQ_C1 : SQ_C8);

	if (can_castle<c, KINGSIDE>()) {
		if (!(attacks & king_side) &&
		    !(get_pieces() & king_side_obstacles))
			legal_moves[king_square] |= king_side_castle;
	}
	if (can_castle<c, QUEENSIDE>()) {
		if (!(attacks & queen_side) &&
		    !(get_pieces() & queen_side_obstacles))
			legal_moves[king_square] |= queen_side_castle;
	}
}
// en passant {{{
template <Color c>
inline void Chessboard::enpassant_moves() {
	if (enpassant == SQ_NONE) return;

	constexpr auto dir = c == WHITE ? 1 : -1;

	const bool is_pawn_right =
	    bb_of(Square(enpassant - 1)) & get_pieces<PAWN, c>();
	const bool is_pawn_left =
	    bb_of(Square(enpassant + 1)) & get_pieces<PAWN, c>();

	if (!is_pawn_right && !is_pawn_left) return;

	if (is_on_row<ROW_A>(enpassant)) {
		if (is_pawn_right)
			legal_moves[enpassant + 1] |=
			    bb_of(Square(enpassant + dir * 8));
		return;
	}
	if (is_on_row<ROW_H>(enpassant)) {
		if (is_pawn_left)
			legal_moves[enpassant - 1] |=
			    bb_of(Square(enpassant + dir * 8));
		return;
	}

	if (is_pawn_right && is_pawn_left) {
		legal_moves[enpassant + 1] |=
		    bb_of(Square(enpassant + dir * 8));
		legal_moves[enpassant - 1] |=
		    bb_of(Square(enpassant + dir * 8));
		return;
	}

	const Bitboard pieces = get_pieces();
	const Bitboard king   = get_pieces<KING, c>();
	const Bitboard sliders =
	    get_pieces<QUEEN, enemy(c)>() | get_pieces<ROOK, enemy(c)>();

	const Bitboard ray_right = is_pawn_right
				       ? ray<EAST>(Square(enpassant + 1))
				       : ray<EAST>(enpassant);
	const Bitboard ray_left  = is_pawn_right
				       ? ray<WEST>(enpassant)
				       : ray<WEST>(Square(enpassant - 1));
	const auto offset        = is_pawn_right ? 1 : -1;

	const Square collision_right =
	    closest_collision<EAST>(ray_right, pieces);
	const Square collision_left =  //
	    closest_collision<WEST>(ray_left, pieces);

	const Bitboard collision_bb_right = bb_of(collision_right);
	const Bitboard collision_bb_left  = bb_of(collision_left);

	if ((collision_bb_left & king) && (collision_bb_right & sliders))
		return;
	if ((collision_bb_left & sliders) && (collision_bb_right & king))
		return;

	legal_moves[enpassant + offset] |= bb_of(Square(enpassant + dir * 8));
}

// piece moves {{{
template <Piece p, Color c>
constexpr Bitboard Chessboard::piece_moves(Square square) {
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
	}
}
/*}}}*/
template <Color c, Direction d>
inline void Chessboard::compute_pin() {
	constexpr Color enemy_c     = enemy(c);
	const Bitboard king         = get_pieces<KING, c>();
	const Bitboard ray_king     = ray<d>(Square(lsb(king)));
	const Bitboard enemy_pieces = get_pieces<enemy_c>();
	const Bitboard ally_pieces  = get_pieces<c>();
	const Square enemy =
	    Square(closest_collision<d>(ray_king, enemy_pieces));
	constexpr bool is_diag = d == NORTH_EAST || d == NORTH_WEST ||
				 d == SOUTH_EAST || d == SOUTH_WEST;
	constexpr Bitboard sliders = get_pieces<QUEEN, enemy_c>() |
				     (is_diag ? get_pieces<BISHOP, enemy_c>()
					      : get_pieces<ROOK, enemy_c>());
	if (!(enemy & sliders)) return;

	const Bitboard ray_enemy = ray<opposite_direction(d)>(enemy);
	const Square first_encounter =
	    Square(closest_collision<d>(ray_king, ally_pieces));
	const Square last_encounter = Square(
	    closest_collision<opposite_direction(d)>(ray_enemy, ally_pieces));
	if (first_encounter == last_encounter) {
		legal_moves[first_encounter] &= ray_enemy;
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
	pieces_moves<KING, c>();
	if (check_count >= 2) return;
	pieces_moves<PAWN, c>();
	pieces_moves<BISHOP, c>();
	pieces_moves<ROOK, c>();
	pieces_moves<QUEEN, c>();
	pieces_moves<KNIGHT, c>();
	king_castle_moves<c>();
	enpassant_moves<c>();
	compute_pins<c>();
	if (check_count == 1) {
		for (int i = 0; i < 64; ++i) {
			legal_moves[i] &= check;
		}
	}
} /*}}}*/

/*}}}*/

/*}}}*/

inline void Chessboard::update_castle(Square rook) {
	switch (rook) {
	case SQ_A1:
		castling = Castling(castling & WHITE_OO);
		break;
	case SQ_A8:
		castling = Castling(castling & WHITE_OOO);
		break;
	case SQ_H1:
		castling = Castling(castling & BLACK_OO);
		break;
	case SQ_H8:
		castling = Castling(castling & BLACK_OOO);
		break;
	default:
		break;
	}
}

// Move {{{
bool Chessboard::move(Square from, Square to, Piece promotion) {
	Color c = Color(turn_count % 2);
	if (!(legal_moves[from] & bb_of(to))) return false;
	Piece piece     = get_piece(from);
	Piece new_piece = piece;
	Piece captured  = get_piece(to);

	if (piece == PAWN) {
		auto dir = c == WHITE ? 1 : -1;
		if (to == enpassant + dir * 8) {
			captured = PAWN;
			color[enemy(c)] &= ~bb_of(enpassant);
			pieces[PAWN] &= ~bb_of(enpassant);
		} else if ((c == WHITE && is_on_line<LINE_8>(to)) ||
			   (c == BLACK && is_on_line<LINE_1>(to))) {
			switch (promotion) {
			case QUEEN:
			case KNIGHT:
			case ROOK:
			case BISHOP:
				new_piece = promotion;
				break;
			default:
				return false;
			}
		}
		if (to == from + dir * 16) {
			enpassant = Square(to);
		} else {
			enpassant = SQ_NONE;
		}
	} else if (piece == KING) {
		if (abs(to - from) == 2) {
			Square rook_from =
			    Square(to > from ? from + 3 : from - 4);
			Square rook_to =
			    Square(to > from ? from + 1 : from - 1);
			pieces[ROOK] &= ~bb_of(rook_from);
			pieces[ROOK] |= bb_of(rook_to);
			color[c] &= ~bb_of(rook_from);
			color[c] |= bb_of(rook_to);
		}
		castling = Castling(
		    castling & ~(c == WHITE ? WHITE_CASTLE : BLACK_CASTLE));
	} else if (piece == ROOK) {
		update_castle(from);
	}

	if (captured != NONE) {
		if (captured == ROOK) {
			update_castle(to);
		}

		pieces[captured] &= ~bb_of(to);
		color[enemy(c)] &= ~bb_of(to);
	}
	pieces[piece] &= ~bb_of(from);
	color[c] &= ~bb_of(from);
	pieces[new_piece] |= bb_of(to);
	color[c] |= bb_of(to);

	return true;
}
/*}}}*/

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

int main() { return 0; }
