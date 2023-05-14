#include "player/player_random.hpp"

#include <random>
#include <stdexcept>

#include "player/player.hpp"

void Player_random::start_new_game(bool is_white) {
	this->is_white   = is_white;
	this->is_started = true;
}

Player_move Player_random::play(Chessboard chessboard) {
	std::vector<board::Move> moves = chessboard.get_all_legal_moves();
	std::uniform_int_distribution<> distribution(0, moves.size() - 1);
	return Player_move{PLAY, moves[distribution(generator)]};
}

Player_move Player_random::invalid_move(Chessboard chessboard) {
	(void)chessboard;
	throw std::runtime_error("Unexpected invalid_move");
}

void Player_random::end() { this->is_started = false; }
