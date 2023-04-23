#pragma once

#include "player/player.hpp"
#include "view/view.hpp"

class Controller {
       public:
	Controller(Player player1, Player player2, View view, std::string pgn);
	Controller(Player player1, Player player2, View view);
	virtual ~Controller() = 0;

	virtual Controller& start() = 0;
	virtual Controller& reset() = 0;
};