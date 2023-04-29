#include "board.hpp"
#include "controller/controller.hpp"
#include "logic/chessboard.hpp"
#include "player/tui.hpp"
#include "view/multiplexer.hpp"
#include "view/tui.hpp"

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	Controller controller(std::make_unique<Player_tui>(),
			      std::make_unique<Player_tui>(),
			      std::make_unique<Demultiplexer<Tui, Tui>>());
	controller.start();
	return 0;
}
