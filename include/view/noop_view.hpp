#pragma once

#include "view/view.hpp"

class View_noop : public View {
       public:
	View_noop() = default;
	void start_new_game(cboard::Chessboard board) override { (void)board; }
	void update(cboard::Chessboard board) override { (void)board; }
	void end() override {}
};
