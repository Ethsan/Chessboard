#pragma once

#include <memory>

#include "logic/chessboard.hpp"
#include "player/player.hpp"
#include "view/noop_view.hpp"

class Controller {
       private:
	std::unique_ptr<Player> white;
	std::unique_ptr<Player> black;
	std::unique_ptr<View> view;
	Chessboard chessboard;

       public:
	Controller(std::unique_ptr<Player> white, std::unique_ptr<Player> black,
		   std::unique_ptr<View> view,
		   board::Board board = board::initial_board)
	    : white(std::move(white)),
	      black(std::move(black)),
	      view(std::move(view)),
	      chessboard(board){};
	Controller(std::unique_ptr<Player> white, std::unique_ptr<Player> black,
		   board::Board board = board::initial_board)
	    : white(std::move(white)),
	      black(std::move(black)),
	      view(std::make_unique<View_noop>()),
	      chessboard(board){};

	void start();
};
