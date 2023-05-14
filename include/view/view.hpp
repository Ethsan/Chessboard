#pragma once

#include "logic/chessboard.hpp"

/**
 * @brief Class that render the game
 */
class View {
       public:
	View() = default;
	View(Chessboard);
	virtual ~View() = default;

	/**
	 * @brief First method called by the controller
	 *
	 * @param board Chessboard Object
	 */
	virtual void start_new_game(Chessboard board) = 0;
	/**
	 * @brief Method called by the controller after each move
	 *
	 * @param board Chessboard Object
	 */
	virtual void update(Chessboard board) = 0;
	/**
	 * @brief Method called by the controller when the game ends
	 *
	 */
	virtual void end() = 0;
};
