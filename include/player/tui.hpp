#pragma once

#include "player/player.hpp"

class Player_tui : public Player {
	bool parse_move(std::string input, Move& move);
	bool is_white;

       public:
	Player_tui(bool is_white) : is_white(is_white) {}
	~Player_tui() override = default;

	Player_tui& reset(bool is_white) override;
	Move play(cboard::Chessboard chessboard) override;
	Move invalid_move(cboard::Chessboard chessboard) override;
	Player_tui& end() override;
};
