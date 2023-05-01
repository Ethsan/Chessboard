#pragma once

#include "logic/chessboard.hpp"
#include "view/view.hpp"

class View_noop : public View {
       public:
	View_noop() = default;
	View_noop(Chessboard){};
	void start_new_game(Chessboard board) override { (void)board; }
	void update(Chessboard board) override { (void)board; }
	void end() override {}
};
