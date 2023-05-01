#pragma once

#include <array>
#include <cassert>
#include <string>
#include <vector>

#include "board.hpp"
#include "logic/bitboard.hpp"

namespace logic {
enum Side {
	KINGSIDE,
	QUEENSIDE,
};

enum Castling {
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

enum Color {
	WHITE,
	BLACK,
	COLOR_NONE,
};

enum Piece {
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
	PIECE_NONE,
};
}  // namespace logic

enum GameState {
	ONGOING,
	WHITE_CHECKMATE,
	BLACK_CHECKMATE,
	STALEMATE,
};

// class Chessboard {{{
class Chessboard {
	logic::Bitboard color[2];
	logic::Bitboard pieces[6];
	logic::Bitboard attacks;
	logic::Bitboard threat;
	logic::Bitboard legal_moves[64];
	logic::Square enpassant;
	unsigned int turn_count;
	unsigned int legal_move_count;
	unsigned int check_count;
	logic::Castling castling;
	GameState game_state = ONGOING;
	board::Move last_move;

       public:
	Chessboard(const board::Board& board = board::initial_board);

	bool make_move(board::Move move);

	bool is_legal(board::Square from, board::Square to) const;
	bool is_same_as(const Chessboard& chessboard) const;
	bool is_attacked(board::Square square) const;

	board::Colored_piece get_piece(board::Square square) const;

	GameState get_game_state() const;
	void set_game_state(GameState game_state);

	std::vector<board::Move> get_all_legal_moves() const;
	board::Move get_last_move() const { return last_move; };

	board::Board to_array() const;

       private:
	// --- Utils ---
	template <logic::Color c>
	bool can_castle() const;
	template <logic::Color c, logic::Side s>
	bool can_castle() const;
	inline logic::Piece get_piece(logic::Square square) const;
	inline logic::Color get_color(logic::Square square) const;
	template <logic::Piece p>
	inline void fill_array(board::Board& board) const;
	template <logic::Color c>
	inline void fill_array(board::Board& board) const;
	template <logic::Color>

	// --- Move computation ---
	// ++++++++ attack ++++++++
	inline void compute_pawn_attack(logic::Square square);
	template <logic::Direction d, logic::Color c>
	inline void compute_ray_attack(logic::Square square);
	template <logic::Color>
	inline void compute_rook_attack(logic::Square square);
	template <logic::Color>
	inline void compute_bishop_attack(logic::Square square);
	template <logic::Color>
	inline void compute_knight_attack(logic::Square square);
	template <logic::Color>
	inline void compute_queen_attack(logic::Square square);
	inline void compute_king_attack(logic::Square square);

	template <logic::Piece p, logic::Color c>
	inline void compute_attack(logic::Square square);
	template <logic::Piece p, logic::Color c>
	inline void compute_pieces_attack();
	template <logic::Color>
	inline void compute_attacks();

	// ++++++++ move ++++++++
	template <logic::Direction d, logic::Color c>
	inline void compute_ray_moves(logic::Square square);
	template <logic::Color>
	inline void compute_pawn_moves(logic::Square square);
	template <logic::Color>
	inline void compute_rook_moves(logic::Square square);
	template <logic::Color>
	inline void compute_bishop_moves(logic::Square square);
	template <logic::Color>
	inline void compute_knight_moves(logic::Square square);
	template <logic::Color>
	inline void compute_queen_moves(logic::Square square);
	template <logic::Color>
	inline void compute_king_moves(logic::Square square);
	template <logic::Piece p, logic::Color c>
	constexpr void compute_piece_moves(logic::Square square);
	template <logic::Color c>
	inline void compute_enpassant();
	template <logic::Color>
	inline void compute_castling();

	template <logic::Color c, logic::Direction d>
	inline void compute_pin();
	template <logic::Color c>
	inline void compute_pins();
	template <logic::Piece p, logic::Color c>
	inline void compute_pieces_moves();

	template <logic::Color>
	inline void compute_moves();
	template <logic::Color>
	inline void compute_legal();

	inline void update_castle(logic::Square rook);
};
