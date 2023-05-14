#pragma once

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
	/**
	 * @brief Simple constructor with the initial board
	 *
	 * @param board Initial board
	 */
	Chessboard(const board::Board& board = board::initial_board);

	/**
	 * @brief Check if a move is legal and update the chessboard according
	 * to it
	 *
	 * @param move Move to make
	 * @return Boolean true if the move was legal, false otherwise
	 */
	bool make_move(board::Move move);

	/**
	 * @brief Check if a move is legal in constant time
	 *
	 * @param from origin square
	 * @param to destination square
	 * @return Boolean true if the move is legal, false otherwise
	 */
	bool is_legal(board::Square from, board::Square to) const;
	/**
	 * @brief Method to compare two positions
	 *
	 * @param chessboard Chessboard to compare to
	 * @return true if the two positions are the same, false otherwise
	 */
	bool is_same_as(const Chessboard& chessboard) const;

	/**
	 * @brief Get the current turn count
	 *
	 * @return int
	 */
	int get_turn_count() const { return turn_count; };
	/**
	 * @brief Get the piece on the square
	 *
	 * @param square Square to check
	 * @return board::Colored_piece
	 */
	board::Colored_piece get_piece(board::Square square) const;

	/**
	 * @brief Get the current state of the game
	 *
	 * @return game_state GameState
	 */
	GameState get_game_state() const;
	/**
	 * @brief Set the state of the game in case a player resign or draw
	 *
	 * @param game_state GameState to set
	 */
	void set_game_state(GameState game_state);

	/**
	 * @brief Return the number of legal moves in constant time
	 *
	 * @return int Number of legal moves
	 */
	int get_legal_move_count() const { return legal_move_count; };
	/**
	 * @brief List all legal moves
	 *
	 * @return std::vector<board::Move>
	 */
	std::vector<board::Move> get_all_legal_moves() const;
	/**
	 * @brief Return the last move made
	 *
	 * @return board::Move
	 */
	board::Move get_last_move() const { return last_move; };

	/**
	 * @brief Create a board::Board from the current state of the chessboard
	 *
	 * @return board::Board
	 */
	board::Board to_array() const;

       private:
	// --- Utils ---
	bool is_attacked(board::Square square) const;
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
