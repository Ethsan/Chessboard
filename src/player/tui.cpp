#include "player/tui.hpp"

#include <regex>
#include <stdexcept>

#include "board.hpp"
#include "player/player.hpp"

using namespace std;
using namespace board;

Player_tui& Player_tui::reset(bool is_white) {
	this->is_white = is_white;
	return *this;
}

Piece parse_piece(char input) {
	switch (input) {
	case 'N':
	case 'n':
		return board::KNIGHT;
	case 'R':
	case 'r':
		return board::ROOK;
	case 'B':
	case 'b':
		return board::BISHOP;
	case 'Q':
	case 'q':
		return board::QUEEN;
	case 'K':
	case 'k':
		return board::KING;
	case 'p':
	case 'P':
		return board::PAWN;
	default:
		return board::NO_PIECE;
	}
}

bool Player_tui::parse_move(string input, Move& move) {
	regex king_side_castle("(O|o|0)-(O|o|0)");
	regex queen_side_castle("(O|o|0)-(O|o|0)-(O|o|0)");
	regex movement("[a-h][1-8][a-h][1-8]");
	regex movement_with_promotion("[a-h][1-8][a-h][1-8][RrBbNnQq]");

	if (regex_match(input, movement)) {
		move.from = Square(input.substr(0, 2));
		move.to   = Square(input.substr(2, 2));
	} else if (regex_match(input, movement_with_promotion)) {
		move.from      = Square(input.substr(0, 2));
		move.to        = Square(input.substr(2, 2));
		move.promotion = parse_piece(input[4]);
	} else if (regex_match(input, king_side_castle)) {
		move.from = is_white ? Square("e1") : Square("e8");
		move.to   = is_white ? Square("g1") : Square("g8");
	} else if (regex_match(input, queen_side_castle)) {
		move.from = is_white ? Square("e1") : Square("e8");
		move.to   = is_white ? Square("c1") : Square("c8");
	} else {
		return false;
	}
	move.decision = PLAY;
	return true;
}

Move Player_tui::play(cboard::Chessboard chessboard) {
	(void)chessboard;

	Move move;
	while (true) {
		string input;
		cout << "Coup (eg. a1a8) ? ";
		cin >> input;

		if (input == "/resign") {
			move.decision = RESIGN;
			break;
		} else if (input == "/quit") {
			move.decision = END;
			break;
		} else if (input == "/draw") {
			move.decision = DRAW;
			break;
		} else if (parse_move(input, move)) {
			break;
		} else {
			continue;
		}
	}
	return move;
}

Move Player_tui::invalid_move(cboard::Chessboard chessboard) {
	cout << "Illegal move" << endl;
	return play(chessboard);
}

Player_tui& Player_tui::end() { return *this; }
