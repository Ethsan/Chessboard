#pragma once

#include "board.hpp"
#include "logic/chessboard.hpp"

enum Decision { PLAY, RESIGN, DRAW, END };

struct Move {
	Decision decision;
	board::Square from;
	board::Square to;
	board::Piece promotion = board::NO_PIECE;
};

class Player {
       public:
	Player() = default;
	virtual ~Player() = default;

	virtual void start_new_game(bool is_white)               = 0;
	virtual Move play(cboard::Chessboard chessboard)         = 0;
	virtual Move invalid_move(cboard::Chessboard chessboard) = 0;
	virtual void end()                                       = 0;
};
