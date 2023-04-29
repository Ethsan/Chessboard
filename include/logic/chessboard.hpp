#pragma once

#include <array>
#include <cassert>
#include <string>

#include "board.hpp"
#include "logic/bitboard.hpp"

namespace cboard {

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
};

enum Color : int {
	WHITE,
	BLACK,
	COLOR_NONE,
};

enum Piece : int {
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
	PIECE_NONE,
};

enum GameState : int {
	ONGOING,
	WHITE_CHECKMATE,
	BLACK_CHECKMATE,
	STALEMATE,
};

// class Chessboard {{{
class Chessboard {
	bboard::Bitboard color[2];
	bboard::Bitboard pieces[6];
	bboard::Bitboard attacks;
	bboard::Bitboard threat;
	bboard::Bitboard legal_moves[64];
	bboard::Square enpassant;
	unsigned int turn_count;
	unsigned int legal_move_count;
	unsigned int check_count;
	Castling castling;
	GameState game_state = ONGOING;

       public:
	Chessboard(const board::Board& board = board::initial_board);
	bool make_move(board::Square from, board::Square to,
		  board::Piece promotion = board::NO_PIECE);
	bool is_legal(board::Square from, board::Square to) const;
	template <Color c>
	bool can_castle() const;
	template <Color c, Side s>
	bool can_castle() const;
	bool is_same_as(const Chessboard& chessboard) const;
	bool is_attacked(board::Square square) const;
	board::Colored_piece get_piece(board::Square square) const;
	board::Board to_array() const;
	GameState get_game_state() const;
	void set_game_state(GameState game_state);

       private:
	// --- Utils ---
	inline Piece get_piece(bboard::Square square) const;
	inline Color get_color(bboard::Square square) const;
	template <Piece p>
	inline void fill_array(board::Board& board) const;
	template <Color c>
	inline void fill_array(board::Board& board) const;
	template <Color>

	// --- Move computation ---
	// ++++++++ attack ++++++++
	inline void compute_pawn_attack(bboard::Square square);
	template <bboard::Direction d, Color c>
	inline void compute_ray_attack(bboard::Square square);
	template <Color>
	inline void compute_rook_attack(bboard::Square square);
	template <Color>
	inline void compute_bishop_attack(bboard::Square square);
	template <Color>
	inline void compute_knight_attack(bboard::Square square);
	template <Color>
	inline void compute_queen_attack(bboard::Square square);
	inline void compute_king_attack(bboard::Square square);

	template <Piece p, Color c>
	inline void compute_attack(bboard::Square square);
	template <Piece p, Color c>
	inline void compute_pieces_attack();
	template <Color>
	inline void compute_attacks();

	// ++++++++ move ++++++++
	template <bboard::Direction d, Color c>
	inline void compute_ray_moves(bboard::Square square);
	template <Color>
	inline void compute_pawn_moves(bboard::Square square);
	template <Color>
	inline void compute_rook_moves(bboard::Square square);
	template <Color>
	inline void compute_bishop_moves(bboard::Square square);
	template <Color>
	inline void compute_knight_moves(bboard::Square square);
	template <Color>
	inline void compute_queen_moves(bboard::Square square);
	template <Color>
	inline void compute_king_moves(bboard::Square square);
	template <Piece p, Color c>
	constexpr void compute_piece_moves(bboard::Square square);
	template <Color c>
	inline void compute_enpassant();
	template <Color>
	inline void compute_castling();

	template <Color c, bboard::Direction d>
	inline void compute_pin();
	template <Color c>
	inline void compute_pins();
	template <Piece p, Color c>
	inline void compute_pieces_moves();

	template <Color>
	inline void compute_moves();
	template <Color>
	inline void compute_legal();

	inline void update_castle(bboard::Square rook);
};
};  // namespace cboard
