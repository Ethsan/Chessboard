#include "view/tui.hpp"

#include <cassert>
#include <iostream>
#include <string>

#include "board.hpp"
#include "logic/chessboard.hpp"

using namespace std;
using namespace board;

static const constexpr char bright_bg[] = "\033[47m";
static const constexpr char dark_bg[]   = "\033[40m";
static const constexpr char reset_bg[]  = "\033[49m";

Tui::Tui(cboard::Chessboard chessboard) { update(chessboard); };

Tui& Tui::reset(cboard::Chessboard chessboard) {
	update(chessboard);
	return *this;
}

Tui& Tui::update(cboard::Chessboard chessboard) {
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
	return *this;
}

std::string to_string(cboard::GameState state) {
	switch (state) {
	case cboard::WHITE_TO_PLAY:
	case cboard::BLACK_TO_PLAY:
		return "?-?";
	case cboard::WHITE_CHECKMATE:
		return "1-0";
	case cboard::BLACK_CHECKMATE:
		return "0-1";
	case cboard::STALEMATE:
		return "1/2-1/2";
	}
}

Tui& Tui::end() {
	const Board board = chessboard.to_array();
	for (auto& line : board) {
		for (auto& square : line) {
			cout << square.to_pgn() << ',';
		}
	}
	cout << " " << to_string(chessboard.get_game_state()) << endl;
	return *this;
}
