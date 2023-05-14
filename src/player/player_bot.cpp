#include "player/player_bot.hpp"

#include <array>
#include <iostream>
#include <limits>
#include <ostream>
#include <utility>
#include <vector>

#include "board.hpp"
#include "logic/chessboard.hpp"
#include "player/player.hpp"

using namespace board;

// clang-format off
std::array<int, 64> knight_heatmap = {
    2, 3, 4, 4, 4, 4, 3, 2,
    3, 4, 6, 6, 6, 6, 4, 3,
    4, 6, 8, 8, 8, 8, 6, 4,
    4, 6, 8, 8, 8, 8, 6, 4,
    4, 6, 8, 8, 8, 8, 6, 4,
    4, 6, 8, 8, 8, 8, 6, 4,
    3, 4, 6, 6, 6, 6, 4, 3,
    2, 3, 4, 4, 4, 4, 3, 2};
std::array<int, 64> bishop_heatmap = {
    7, 6, 5, 5, 5, 5, 6, 7,
    6, 5, 4, 4, 4, 4, 5, 6,
    5, 4, 3, 3, 3, 3, 4, 5,
    5, 4, 3, 2, 2, 3, 4, 5,
    5, 4, 3, 2, 2, 3, 4, 5,
    5, 4, 3, 3, 3, 3, 4, 5,
    6, 5, 4, 4, 4, 4, 5, 6,
    7, 6, 5, 5, 5, 5, 6, 7};
// clang-format on

float evaluate(const Chessboard &chessboard) {
	GameState gs = chessboard.get_game_state();
	switch (gs) {
	case ONGOING:
		break;
	case WHITE_CHECKMATE:
		return std::numeric_limits<float>::max();
	case BLACK_CHECKMATE:
		return -std::numeric_limits<float>::max();
	case STALEMATE:
		return 0;
	}

	float evaluation = 0;
	Board board      = chessboard.to_array();
	int square       = 0;

	for (auto &line : board) {
		for (auto &piece : line) {
			int sign = piece.color == WHITE ? 1 : -1;
			int score;
			switch (piece.piece) {
			case PAWN:
				score = 1;
				break;
			case ROOK:
				score = 5;
				break;
			case KNIGHT:
				score =
				    3 + (float)knight_heatmap[square] / 8 * 0.1;
				break;
			case BISHOP:
				score =
				    3 + (float)bishop_heatmap[square] / 8 * 0.1;
				break;
			case QUEEN:
				score = 8;
				break;
			default:
				score = 0;
				break;
			}
			evaluation += sign * score;
			square++;
		}
	}
	return evaluation;
}

std::pair<Move, float> negaMax(Chessboard chessboard, int depth,
			       bool is_player) {
	int sign = is_player ? 1 : -1;

	if (depth == 0) return {Move(), sign * evaluate(chessboard)};
	std::vector<Move> moves = chessboard.get_all_legal_moves();
	if (moves.size() == 0) return {Move(), sign * evaluate(chessboard)};

	Move best_move;
	float best_score = -std::numeric_limits<float>::infinity();

	for (auto &move : moves) {
		Chessboard new_chessboard = chessboard;
		new_chessboard.make_move(move);
		float complexity = (float)new_chessboard.get_legal_move_count() / 200 * 0.01;
		float score =
		    -negaMax(new_chessboard, depth - 1, !is_player).second - complexity;

		if (score > best_score) {
			best_score = score;
			best_move  = move;
		}
	}

	assert(best_score != -std::numeric_limits<float>::infinity());

	return {best_move, best_score};
}

void Player_bot::start_new_game(bool is_white) {
	this->is_white   = is_white;
	this->is_started = true;
}

Player_move Player_bot::play(Chessboard chessboard) {
	Move move = negaMax(chessboard, this->max_depth, is_white).first;
	std::cout << move.from.to_string() << " " << move.to.to_string()
		  << std::endl;
	return {PLAY, move};
}

Player_move Player_bot::invalid_move(Chessboard chessboard) {
	(void)chessboard;
	throw std::runtime_error("Unexpected invalid_move");
}

void Player_bot::end() { this->is_started = false; }
