#pragma once

#include "player/player.hpp"

class Player_bot : public Player {
	bool is_white;
	bool is_started = false;
	int max_depth   = 4;

       public:
	~Player_bot() override = default;

	void start_new_game(bool is_white) override;
	Player_move play(Chessboard chessboard) override;
	Player_move invalid_move(Chessboard chessboard) override;
	void end() override;
};
