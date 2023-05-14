#pragma once

#include <memory>

#include "logic/chessboard.hpp"
#include "player/player.hpp"
#include "view/noop_view.hpp"

/**
 * @brief Controller class. This class is responsible for managing the game
 * flow.
 */
class Controller {
       private:
	std::unique_ptr<Player> white;
	std::unique_ptr<Player> black;
	std::unique_ptr<View> view;
	Chessboard chessboard;

       public:
	/**
	 * @brief Construct a new Controller object
	 *
	 * @param white Player that control white
	 * @param black Player that control black
	 * @param view View that render the game
	 * @param board Initial board
	 */
	Controller(std::unique_ptr<Player> white, std::unique_ptr<Player> black,
		   std::unique_ptr<View> view,
		   board::Board board = board::initial_board)
	    : white(std::move(white)),
	      black(std::move(black)),
	      view(std::move(view)),
	      chessboard(board){};
	/**
	 * @brief Construct a new Controller object without a view to render the
	 * game
	 *
	 * @param white Player that control white
	 * @param black Player that control black
	 * @param board Initial board
	 */
	Controller(std::unique_ptr<Player> white, std::unique_ptr<Player> black,
		   board::Board board = board::initial_board)
	    : white(std::move(white)),
	      black(std::move(black)),
	      view(std::make_unique<View_noop>()),
	      chessboard(board){};

	/**
	 * @brief Start the game
	 *
	 */
	void start();
};
