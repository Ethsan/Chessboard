#pragma once

#include "logic/chessboard.hpp"

enum Action { PLAY, RESIGN, DRAW, END };

struct Player_move {
	Action action;
	board::Move move;
};

class Player {
       public:
	Player()          = default;
	virtual ~Player() = default;

	virtual void start_new_game(bool is_white)              = 0;
	virtual Player_move play(Chessboard chessboard)         = 0;
	virtual Player_move invalid_move(Chessboard chessboard) = 0;
	virtual void end()                                      = 0;
};
