#pragma once

#include "logic/chessboard.hpp"

class View {
       public:
	View() = default;
	View(cboard::Chessboard);
	virtual ~View() = default;

	virtual View& reset(cboard::Chessboard board)  = 0;
	virtual View& update(cboard::Chessboard board) = 0;
	virtual View& end()                            = 0;
};
