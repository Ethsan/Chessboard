#include "logic/chessboard.hpp"

#include <iostream>
#include <ostream>

#include "board.hpp"
#include "logic/bitboard.hpp"

using namespace cboard;
using namespace bboard;

Square convert(board::Square square) {
	return Square(square.row + square.line * 8);
}

board::Square convert(Square square) {
	return board::Square{static_cast<board::Line>(square / 8),
			     static_cast<board::Row>(square % 8)};
}

constexpr Piece convert(board::Piece piece) {
	switch (piece) {
	case board::PAWN:
		return PAWN;
	case board::ROOK:
		return ROOK;
	case board::KNIGHT:
		return KNIGHT;
	case board::BISHOP:
		return BISHOP;
	case board::QUEEN:
		return QUEEN;
	case board::KING:
		return KING;
	default:
		return PIECE_NONE;
	}
}

constexpr board::Piece convert(Piece piece) {
	switch (piece) {
	case PAWN:
		return board::PAWN;
	case ROOK:
		return board::ROOK;
	case KNIGHT:
		return board::KNIGHT;
	case BISHOP:
		return board::BISHOP;
	case QUEEN:
		return board::QUEEN;
	case KING:
		return board::KING;
	default:
		return board::NO_PIECE;
	}
}

constexpr Color convert(board::Color color) {
	switch (color) {
	case board::WHITE:
		return WHITE;
	case board::BLACK:
		return BLACK;
	default:
		return COLOR_NONE;
	}
}

constexpr board::Color convert(Color color) {
	switch (color) {
	case WHITE:
		return board::WHITE;
	case BLACK:
		return board::BLACK;
	default:
		return board::NO_COLOR;
	}
}
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

Chessboard::Chessboard(const board::Board &board) {
	for (auto &bitboard : pieces) bitboard = BOARD_CLEAR;
	for (auto &bitboard : color) bitboard = BOARD_CLEAR;

	int i = 0;
	for (auto &line : board) {
		for (auto &square : line) {
			if (square.piece != board::NO_PIECE) {
				pieces[convert(square.piece)] |= (Bitboard)1
								 << i;
				color[convert(square.color)] |= (Bitboard)1
								<< i;
			}
			i++;
		}
	}

	turn_count = 0;
	castling   = ALL;
	enpassant  = SQ_NONE;
	compute_legal<WHITE>();
};

constexpr Color enemy(Color color) {
	switch (color) {
	case WHITE:
		return BLACK;
	case BLACK:
		return WHITE;
	default:
		assert(false);
	}
};

bool Chessboard::is_same_as(const Chessboard &chessboard) const {
	return color[WHITE] == chessboard.color[WHITE] &&
	       color[BLACK] == chessboard.color[BLACK] &&
	       pieces[PAWN] == chessboard.pieces[PAWN] &&
	       pieces[ROOK] == chessboard.pieces[ROOK] &&
	       pieces[KNIGHT] == chessboard.pieces[KNIGHT] &&
	       pieces[BISHOP] == chessboard.pieces[BISHOP] &&
	       pieces[QUEEN] == chessboard.pieces[QUEEN] &&
	       pieces[KING] == chessboard.pieces[KING] &&
	       castling == chessboard.castling &&
	       enpassant == chessboard.enpassant;
}

bool Chessboard::is_attacked(board::Square bsquare) const {
	Square square = convert(bsquare);
	return attacks & bb_of(square);
}

template <Color c>
inline void Chessboard::fill_array(board::Board &board) const {
	auto pieces_c = color[c];
	while (pieces_c) {
		auto square = pop_lsb(pieces_c);

		board[square / 8][square % 8].color = convert(c);
	}
}

template <Piece p>
inline void Chessboard::fill_array(board::Board &board) const {
	auto pieces_p = pieces[p];
	while (pieces_p) {
		auto square = pop_lsb(pieces_p);

		board[square / 8][square % 8].piece = convert(p);
	}
}

board::Board Chessboard::to_array() const {
	board::Board board = board::Board();
	fill_array<PAWN>(board);
	fill_array<ROOK>(board);
	fill_array<KNIGHT>(board);
	fill_array<BISHOP>(board);
	fill_array<QUEEN>(board);
	fill_array<KING>(board);
	fill_array<WHITE>(board);
	fill_array<BLACK>(board);

	return board;
}

void Chessboard::set_game_state(GameState game_state) {
	GameState real_state = get_game_state();
	if (real_state == ONGOING) {
		this->game_state = game_state;
	} else {
		this->game_state = real_state;
	}
}

GameState Chessboard::get_game_state() const {
	if (game_state != ONGOING) return game_state;

	if (legal_move_count == 0) {
		if (check_count > 0) {
			return (turn_count % 2 == WHITE ? BLACK_CHECKMATE
							: WHITE_CHECKMATE);
		} else {
			return STALEMATE;
		}
	}
	return ONGOING;
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

board::Colored_piece Chessboard::get_piece(board::Square square) const {
	return board::Colored_piece(convert(get_piece(convert(square))),
				    convert(get_color(convert(square))));
}

Piece Chessboard::get_piece(Square square) const {
	if (pieces[PAWN] & bb_of(square)) return PAWN;
	if (pieces[ROOK] & bb_of(square)) return ROOK;
	if (pieces[KNIGHT] & bb_of(square)) return KNIGHT;
	if (pieces[BISHOP] & bb_of(square)) return BISHOP;
	if (pieces[QUEEN] & bb_of(square)) return QUEEN;
	if (pieces[KING] & bb_of(square)) return KING;
	return PIECE_NONE;
}

Color Chessboard::get_color(Square square) const {
	if (color[WHITE] & bb_of(square)) return WHITE;
	if (color[BLACK] & bb_of(square)) return BLACK;
	return COLOR_NONE;
}

template <Color c>
inline Bitboard Chessboard::pawn_attack(Square square) {
	constexpr int dir = c == WHITE ? -1 : 1;

	const Bitboard pawn = bb_of(square);
	const Line line     = line_of(square);
	const Bitboard king = pieces[KING] & color[c];

	Bitboard attack = safe_bb_of(Square(square + dir * 7)) |
			  safe_bb_of(Square(square + dir * 9));

	attack &= bb_of(Line(line + dir));

	check |= bool(attack & king) * pawn;

	return attack;
}

template <Direction d, Color c>
inline Bitboard Chessboard::ray_attack(Square square) {
	const Bitboard piece     = bb_of(square);
	const Bitboard king      = pieces[KING] & color[c];
	const Bitboard obstacles = (color[WHITE] | color[BLACK]) & ~piece;

	const Bitboard attack = ray_between<d>(square, obstacles);
	check |= bool(attack & king) * attack;
	return attack & ~piece;
}

template <Color c>
inline Bitboard Chessboard::rook_attack(Square square) {
	Bitboard attacks;

	attacks = ray_attack<NORTH, c>(square);
	attacks |= ray_attack<SOUTH, c>(square);
	attacks |= ray_attack<EAST, c>(square);
	attacks |= ray_attack<WEST, c>(square);

	return attacks;
}

template <Color c>
inline Bitboard Chessboard::bishop_attack(Square square) {
	Bitboard attacks;

	attacks = ray_attack<NORTH_EAST, c>(square);
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
	const Bitboard king = pieces[KING] & color[c];

	Bitboard remaining = pieces[p] & color[enemy(c)];
	while (remaining) {
		const Square piece       = Square(pop_lsb(remaining));
		const Bitboard bb_attack = piece_attack<p, c>(piece);
		attacks |= bb_attack;
		check_count += (bool(bb_attack & king));
	}
}

template <Color c>
inline Bitboard Chessboard::knight_attack(Square square) {
	const Bitboard king = pieces[KING] & color[c];
	const Line line     = line_of(square);
	const Column column = row_of(square);

	const Bitboard line_inner =
	    safe_bb_of(Line(line - 1)) | safe_bb_of(Line(line + 1));
	const Bitboard line_outer =
	    safe_bb_of(Line(line - 2)) | safe_bb_of(Line(line + 2));
	const Bitboard col_inner =
	    safe_bb_of(Column(column - 1)) | safe_bb_of(Column(column + 1));
	const Bitboard col_outer =
	    safe_bb_of(Column(column - 2)) | safe_bb_of(Column(column + 2));

	const Bitboard attack =
	    (line_inner & col_outer) | (line_outer & col_inner);

	check |= bb_of(square) * bool(attack & king);
	return attack;
}

inline Bitboard Chessboard::king_attack(Square square) {
	const Line line     = line_of(square);
	const Column column = row_of(square);

	const Bitboard lines = safe_bb_of(Line(line - 1)) | bb_of(Line(line)) |
			       safe_bb_of(Line(line + 1));
	const Bitboard columns = safe_bb_of(Column(column - 1)) |
				 bb_of(Column(column)) |
				 safe_bb_of(Column(column + 1));

	return (lines & columns) & ~bb_of(square);
}

template <Color c>
inline void Chessboard::enemy_attacks() {
	attacks     = BOARD_CLEAR;
	check       = BOARD_CLEAR;
	check_count = 0;

	pieces_attack<PAWN, c>();
	pieces_attack<ROOK, c>();
	pieces_attack<KNIGHT, c>();
	pieces_attack<BISHOP, c>();
	pieces_attack<QUEEN, c>();

	const Square enemy_king = Square(lsb(pieces[KING] & color[enemy(c)]));
	const Bitboard bb_king_attack = king_attack(enemy_king);
	attacks |= bb_king_attack;
}

template <Color c>
constexpr bool isOnLine2(Square square) {
	if (c == BLACK)
		return square >= 48;
	else
		return square < 16;
}

template <Color c>
inline Bitboard Chessboard::pawn_moves(Square square) {
	constexpr int dir         = c == WHITE ? 1 : -1;
	const Bitboard all_pieces = color[WHITE] | color[BLACK];
	const Line line           = line_of(square);

	Bitboard attack_moves = BOARD_CLEAR;
	attack_moves |= bb_of(Square(square + dir * 7));
	attack_moves |= bb_of(Square(square + dir * 9));
	attack_moves &= bb_of(Line(line + dir)) & color[enemy(c)];

	Bitboard push_moves = BOARD_CLEAR;
	push_moves |= bb_of(Square(square + dir * 8));
	push_moves |=
	    bb_of(Square(square + dir * 16)) * isOnLine2<c>(square) *
	    !(bb_of(Square(square + dir * 8)) & (color[WHITE] & color[BLACK]));
	push_moves &= ~all_pieces;

	return attack_moves | push_moves;
}

template <Direction d, Color c>
inline Bitboard Chessboard::ray_moves(Square square) {
	const Bitboard piece      = bb_of(square);
	const Bitboard all_pieces = color[BLACK] | color[WHITE];
	const Bitboard obstacles  = (all_pieces & ~piece) | walls[d];

	return ray_between<d>(square, obstacles);
}

template <Color c>
inline Bitboard Chessboard::rook_moves(Square square) {
	const Bitboard allies = color[c];

	Bitboard moves = BOARD_CLEAR;
	moves |= ray_moves<NORTH, c>(square);
	moves |= ray_moves<SOUTH, c>(square);
	moves |= ray_moves<EAST, c>(square);
	moves |= ray_moves<WEST, c>(square);
	return moves & ~allies;
}

template <Color c>
inline Bitboard Chessboard::bishop_moves(Square square) {
	const Bitboard allies = color[c];

	Bitboard moves = BOARD_CLEAR;
	moves |= ray_moves<NORTH_EAST, c>(square);
	moves |= ray_moves<SOUTH_EAST, c>(square);
	moves |= ray_moves<NORTH_WEST, c>(square);
	moves |= ray_moves<SOUTH_WEST, c>(square);
	return moves & ~allies;
}

template <Color c>
inline Bitboard Chessboard::queen_moves(Square square) {
	return bishop_moves<c>(square) | rook_moves<c>(square);
}

template <Color c>
inline Bitboard Chessboard::knight_moves(Square square) {
	const Bitboard allies = color[c];
	const Line line       = line_of(square);
	const Column column   = row_of(square);

	const Bitboard line_inner =
	    safe_bb_of(Line(line - 1)) | safe_bb_of(Line(line + 1));
	const Bitboard line_outer =
	    safe_bb_of(Line(line - 2)) | safe_bb_of(Line(line + 2));
	const Bitboard col_inner =
	    safe_bb_of(Column(column - 1)) | safe_bb_of(Column(column + 1));
	const Bitboard col_outer =
	    safe_bb_of(Column(column - 2)) | safe_bb_of(Column(column + 2));

	return ((line_inner & col_outer) | (line_outer & col_inner)) & ~allies;
}

template <Color c>
inline Bitboard Chessboard::king_moves(Square square) {
	const Bitboard allies = color[c];

	const Line line     = line_of(square);
	const Column column = row_of(square);

	const Bitboard lines = safe_bb_of(Line(line - 1)) | bb_of(Line(line)) |
			       safe_bb_of(Line(line + 1));
	const Bitboard columns = safe_bb_of(Column(column - 1)) |
				 bb_of(Column(column)) |
				 safe_bb_of(Column(column + 1));

	return (lines & columns) & ~allies & ~attacks;
}

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

	const Bitboard all_pieces = color[BLACK] | color[WHITE];

	if (can_castle<c, KINGSIDE>()) {
		if (!(attacks & king_side) &&
		    !(all_pieces & king_side_obstacles)) {
			legal_moves[king_square] |= king_side_castle;
		}
	}
	if (can_castle<c, QUEENSIDE>()) {
		if (!(attacks & queen_side) &&
		    !(all_pieces & queen_side_obstacles)) {
			legal_moves[king_square] |= queen_side_castle;
		}
	}
}

template <Color c>
inline void Chessboard::enpassant_moves() {
	if (enpassant == SQ_NONE) return;
	std::cout << "enpassant" << std::endl;

	constexpr auto dir = c == WHITE ? 1 : -1;

	const Bitboard pawn_allies = pieces[PAWN] & color[c];

	const bool is_pawn_right =
	    bool(bb_of(Square(enpassant + 1)) & pawn_allies);
	const bool is_pawn_left =
	    bool(bb_of(Square(enpassant - 1)) & pawn_allies);

	if (!is_pawn_right && !is_pawn_left) return;

	if (is_on_row<COL_A>(enpassant)) {
		if (is_pawn_right)
			legal_moves[enpassant + 1] |=
			    bb_of(Square(enpassant + dir * 8));
		return;
	}
	if (is_on_row<COL_H>(enpassant)) {
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

	const Bitboard all_pieces = color[WHITE] | color[BLACK];
	const Bitboard king       = pieces[KING] & color[c];
	const Bitboard sliders =
	    (pieces[QUEEN] | pieces[ROOK]) & color[enemy(c)];

	const Square right = is_pawn_right ? Square(enpassant + 1) : enpassant;
	const Square left  = is_pawn_right ? enpassant : Square(enpassant + 1);

	const auto offset = is_pawn_right ? 1 : -1;

	const Bitboard collision_right = ray_between<EAST>(right, all_pieces);
	bb_print(collision_right);
	const Bitboard collision_left = ray_between<WEST>(left, all_pieces);
	bb_print(collision_left);

	if ((collision_left & king) && (collision_right & sliders)) return;
	if ((collision_left & sliders) && (collision_right & king)) return;

	legal_moves[enpassant + offset] |= bb_of(Square(enpassant + dir * 8));
}

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

template <Piece p, Color c>
inline void Chessboard::pieces_moves() {
	Bitboard remaining = pieces[p] & color[c];
	while (remaining) {
		const Square piece   = Square(pop_lsb(remaining));
		const Bitboard moves = piece_moves<p, c>(piece);
		legal_moves[piece]   = moves;
	}
}

template <Color c, Direction d>
inline void Chessboard::compute_pin() {
	constexpr bool is_diag = d == NORTH_EAST || d == NORTH_WEST ||
				 d == SOUTH_EAST || d == SOUTH_WEST;

	const Bitboard king     = pieces[KING] & color[c];
	const Bitboard ray_king = ray<d>(Square(lsb(king)));
	const Bitboard enemies  = color[enemy(c)];
	const Bitboard allies   = color[c] & ~king;
	const Square enemy = Square(closest_collision<d>(ray_king, enemies));
	Bitboard sliders =
	    (pieces[QUEEN] | (is_diag ? pieces[BISHOP] : pieces[ROOK])) &
	    enemies;
	if (!(bb_of(enemy) & sliders)) return;

	const Bitboard ray_enemy = ray<opposite(d)>(enemy);
	const Square first_encounter =
	    Square(closest_collision<d>(ray_king, allies & ~king));
	const Square last_encounter =
	    Square(closest_collision<opposite(d)>(ray_enemy, allies));

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

template <Color c>
inline void Chessboard::compute_moves() {
	std::fill(legal_moves, legal_moves + 64, BOARD_CLEAR);
	if (check_count < 2) {
		pieces_moves<PAWN, c>();
		pieces_moves<BISHOP, c>();
		pieces_moves<ROOK, c>();
		pieces_moves<QUEEN, c>();
		pieces_moves<KNIGHT, c>();
		enpassant_moves<c>();
		compute_pins<c>();
		if (check_count == 1) {
			for (auto &moves : legal_moves) {
				moves &= check;
			}
		}
	}
	pieces_moves<KING, c>();
	king_castle_moves<c>();
}

template <Color c>
inline void Chessboard::compute_legal() {
	enemy_attacks<c>();
	compute_moves<c>();
	legal_move_count = 0;
	for (auto &moves : legal_moves) {
		legal_move_count += popcount(moves);
	}
}

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

bool Chessboard::move(board::Square b_from, board::Square b_to,
		      board::Piece b_promotion) {
	Piece promotion = convert(b_promotion);
	Square from     = convert(b_from);
	Square to       = convert(b_to);

	Color c = Color(turn_count % 2);
	bb_print(legal_moves[from]);
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
	if (piece != PAWN) {
		enpassant = SQ_NONE;
	}

	if (captured != PIECE_NONE) {
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
	turn_count++;

	if (turn_count % 2 == WHITE) {
		compute_legal<WHITE>();
	} else {
		compute_legal<BLACK>();
	}
	std::cout << "Move count :" << legal_move_count << std::endl;
	auto count = 0;
	for (auto &moves : legal_moves) {
		std::cout << count++ << " : ";
		if (moves) bb_print(moves);
		std::cout << std::endl;
	}

	return true;
}
