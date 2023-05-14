#pragma once

#include "player/player.hpp"

class Player_tui : public Player {
	bool parse_move(std::string input, board::Move &move);
	bool is_white;
	bool is_started = false;

       public:
	Player_tui()                              = default;
	Player_tui(const Player_tui &)            = delete;
	Player_tui(Player_tui &&)                 = delete;
	Player_tui &operator=(const Player_tui &) = delete;
	Player_tui &operator=(Player_tui &&)      = delete;
	~Player_tui() override                    = default;

	void start_new_game(bool is_white) override;
	Player_move play(Chessboard chessboard) override;
	Player_move invalid_move(Chessboard chessboard) override;
	void end() override;
};
