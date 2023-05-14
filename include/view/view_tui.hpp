#pragma once

#include "view/view.hpp"

/**
 * @brief View that render the game in the terminal
 */
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

	/**
	 * @brief Print the chessboard in the terminal
	 *
	 * @param chessboard Chessboard object
	 */
	void start_new_game(Chessboard chessboard) override;
	/**
	 * @brief Print the chessboard in the terminal
	 *
	 * @param chessboard Chessboard object
	 */
	void update(Chessboard chessboard) override;
	/**
	 * @brief Print the the final position and the score in the terminal
	 *
	 */
	void end() override;
};
