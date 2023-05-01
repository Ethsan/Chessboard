#pragma once

#include "logic/chessboard.hpp"

class View {
       public:
	View() = default;
	View(Chessboard);
	virtual ~View() = default;

	virtual void start_new_game(Chessboard board) = 0;
	virtual void update(Chessboard board)         = 0;
	virtual void end()                            = 0;
};
