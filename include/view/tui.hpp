#pragma once

#include "logic/chessboard.hpp"
#include "view.hpp"

class Tui : public View {
       private:
	cboard::Chessboard chessboard;

       public:
	Tui()           = default;
	~Tui() override = default;

	void start_new_game(cboard::Chessboard chessboard) override;
	void update(cboard::Chessboard chessboard) override;
	void end() override;
};
