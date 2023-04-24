#pragma once

#include "logic/chessboard.hpp"
#include "view.hpp"

class Tui : public View {
       private:
	cboard::Chessboard chessboard;

       public:
	Tui();
	Tui(cboard::Chessboard chessboard);

	Tui& reset(cboard::Chessboard chessboard) override;
	Tui& update(cboard::Chessboard chessboard) override;
	Tui& end() override;

	~Tui() override = default;
};
