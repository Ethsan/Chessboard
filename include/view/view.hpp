#pragma once

#include "logic/chessboard.hpp"

class View {
       public:
	View() = default;
	View(cboard::Chessboard);
	virtual ~View() = default;

	virtual void start_new_game(cboard::Chessboard board) = 0;
	virtual void update(cboard::Chessboard board)         = 0;
	virtual void end()                                    = 0;
};
