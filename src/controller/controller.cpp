#include "controller/controller.hpp"

#include "logic/chessboard.hpp"
#include "player/player.hpp"

using namespace cboard;

void Controller::start() {
	white->start_new_game(true);
	black->start_new_game(false);
	view->start_new_game(chessboard);

	int turn        = 0;
	bool is_invalid = false;
	while (chessboard.get_game_state() == ONGOING) {
		auto player = turn % 2 == 0 ? white.get() : black.get();
		Move move   = is_invalid ? player->invalid_move(chessboard)
					 : player->play(chessboard);
		if (move.decision == PLAY) {
			if (!chessboard.make_move(move.from, move.to,
					     move.promotion)) {
				is_invalid = true;
				continue;
			}
			view->update(chessboard);
			is_invalid = false;
			turn++;

		} else if (move.decision == END) {
			chessboard.set_game_state(cboard::ONGOING);
			break;
		} else if (move.decision == DRAW) {
			chessboard.set_game_state(cboard::STALEMATE);
		} else if (move.decision == RESIGN) {
			chessboard.set_game_state(
			    turn % 2 == 0 ? BLACK_CHECKMATE : WHITE_CHECKMATE);
		}
	}

	white->end();
	black->end();
	view->end();
}
