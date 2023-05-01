#pragma once

#include "logic/chessboard.hpp"
#include "view.hpp"

class Tui : public View {
       private:
	Chessboard chessboard;

       public:
	Tui()           = default;
	~Tui() override = default;

	void start_new_game(Chessboard chessboard) override;
	void update(Chessboard chessboard) override;
	void end() override;
};
