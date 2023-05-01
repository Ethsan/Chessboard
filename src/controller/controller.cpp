#include "controller/controller.hpp"

#include "logic/chessboard.hpp"
#include "player/player.hpp"

void Controller::start() {
	white->start_new_game(true);
	black->start_new_game(false);
	view->start_new_game(chessboard);

	int turn        = 0;
	bool is_invalid = false;
	while (chessboard.get_game_state() == ONGOING) {
		auto player = turn % 2 == 0 ? white.get() : black.get();
		Player_move player_move = is_invalid
					      ? player->invalid_move(chessboard)
					      : player->play(chessboard);

		Action action = player_move.action;
		if (action == PLAY) {
			if (!chessboard.make_move(player_move.move)) {
				is_invalid = true;
				continue;
			}
			view->update(chessboard);
			is_invalid = false;
			turn++;

		} else if (action == END) {
			chessboard.set_game_state(ONGOING);
			break;
		} else if (action == DRAW) {
			chessboard.set_game_state(STALEMATE);
		} else if (action == RESIGN) {
			chessboard.set_game_state(
			    turn % 2 == 0 ? BLACK_CHECKMATE : WHITE_CHECKMATE);
		}
	}

	white->end();
	black->end();
	view->end();
}
