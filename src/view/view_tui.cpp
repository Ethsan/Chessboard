#include "view/view_tui.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "board.hpp"
#include "logic/chessboard.hpp"

using namespace std;
using namespace board;

static const constexpr char bright_bg[] = "\033[47m";
static const constexpr char dark_bg[]   = "\033[40m";
static const constexpr char reset_bg[]  = "\033[49m";

std::string to_string(GameState state) {
	switch (state) {
	case ONGOING:
		return "?-?";
	case WHITE_CHECKMATE:
		return "1-0";
	case BLACK_CHECKMATE:
		return "0-1";
	case STALEMATE:
		return "1/2-1/2";
	default:
		throw std::invalid_argument("Incorrect GameState");
	}
}

void View_tui::start_new_game(Chessboard chessboard) { update(chessboard); }

void View_tui::update(Chessboard chessboard) {
	const Board board = chessboard.to_array();
	cout << reset_bg << " ABCDEFGH" << endl;
	char line = '8';
	int count = 0;
	for (auto rit = board.crbegin(); rit != board.rend(); ++rit) {
		count++;
		cout << line;
		line--;
		for (auto piece : *rit) {
			count++;
			cout << (count % 2 ? bright_bg : dark_bg)
			     << piece.to_string();
		}
		cout << reset_bg << endl;
	}

	this->chessboard = std::move(chessboard);
}

void View_tui::end() {
	const Board board = chessboard.to_array();
	cout << endl;
	for (auto& line : board) {
		for (auto& square : line) {
			cout << square.to_pgn() << ',';
		}
	}
	cout << " " << to_string(chessboard.get_game_state()) << endl;
}
