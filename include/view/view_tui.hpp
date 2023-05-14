#pragma once

#include "view/view.hpp"

class View_tui : public View {
       private:
	Chessboard chessboard;

       public:
	View_tui()                            = default;
	View_tui(const View_tui &)            = delete;
	View_tui(View_tui &&)                 = delete;
	View_tui &operator=(const View_tui &) = delete;
	View_tui &operator=(View_tui &&)      = delete;
	~View_tui() override                  = default;

	void start_new_game(Chessboard chessboard) override;
	void update(Chessboard chessboard) override;
	void end() override;
};
