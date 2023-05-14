#include "player/player_tui.hpp"

#include <iostream>
#include <regex>
#include <stdexcept>

#include "board.hpp"

using namespace std;
using namespace board;

void Player_tui::start_new_game(bool is_white) {
	this->is_white = is_white;
	is_started     = true;
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
	case 'P':
	case 'p':
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
	} else if (regex_match(input, king_side_castle)) {
		move.from = is_white ? Square("e1") : Square("e8");
		move.to   = is_white ? Square("g1") : Square("g8");
	} else if (regex_match(input, queen_side_castle)) {
		move.from = is_white ? Square("e1") : Square("e8");
		move.to   = is_white ? Square("c1") : Square("c8");
	} else {
		return false;
	}
	return true;
}

Player_move Player_tui::play(Chessboard chessboard) {
	if (!is_started) {
		throw runtime_error(
		    "Player_tui::play() called before "
		    "Player_tui::start_new_game()");
	}

	Player_move player_move;
	Move& move = player_move.move;
	cout << "Coup (eg. a1a8) ? ";
	while (true) {
		string input;
		cin >> input;

		if (input == "/resign") {
			player_move.action = RESIGN;
			break;
		} else if (input == "/quit") {
			player_move.action = END;
			break;
		} else if (input == "/draw") {
			player_move.action = DRAW;
			break;
		} else if (parse_move(input, move)) {
			player_move.action = PLAY;

			cout << "move: " << move.from.to_string()
			     << move.to.to_string() << endl;

			Colored_piece piece = chessboard.get_piece(move.from);
			if (piece.piece == PAWN &&
			    piece.color == (is_white ? WHITE : BLACK) &&
			    move.to.line == (is_white ? LINE_8 : LINE_1)) {
				cout << "Promotion (N, B, R, Q) ? ";
				string input;
				cin >> input;
				cout << "input: " << input << endl;
				move.promotion = parse_piece(input[0]);
			}
			break;
		} else {
			continue;
		}
	}
	return player_move;
}

Player_move Player_tui::invalid_move(Chessboard chessboard) {
	if (!is_started) {
		throw runtime_error(
		    "Player_tui::invalid_move() called before "
		    "Player_tui::start_new_game()");
	}

	cout << "Illegal move" << endl;
	return play(chessboard);
}

void Player_tui::end() { is_started = false; }
