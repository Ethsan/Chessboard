#pragma once

#include "logic/chessboard.hpp"

class View {
       public:
	View();
	View(cboard::Chessboard);
	View(const View&) = default;

	virtual View& update(cboard::Chessboard board) = 0;
	virtual View& render() const                   = 0;
	virtual ~View()                                = default;
};
