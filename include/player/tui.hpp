#pragma once

#include "player/player.hpp"

class Player_tui : public Player {
	bool parse_move(std::string input, Move& move);
	bool is_white;
	bool is_started = false;

       public:
	~Player_tui() override = default;

	void start_new_game(bool is_white) override;
	Move play(cboard::Chessboard chessboard) override;
	Move invalid_move(cboard::Chessboard chessboard) override;
	void end() override;
};
