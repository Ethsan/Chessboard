#include "board.hpp"

#include <cassert>

using namespace board;

Board from_pgn(const std::string& pgn) {
	const std::string delimiter = ",";
	Board board;

	size_t offset = 0;
	for (auto& line : board) {
		for (auto& square : line) {
			size_t pos = pgn.find(delimiter, offset);
			if (pos == std::string::npos)
				throw std::invalid_argument(
				    "PGN string must have 64 squares");
			std::string token = pgn.substr(offset, pos);
			offset += pos + delimiter.length();
			square = Colored_piece(token);
		}
	}
	if (offset != pgn.length()) {
		throw std::invalid_argument("PGN string must have 64 squares");
	}
	return board;
}

std::string to_pgn(const Board& board) {
	std::string pgn;
	for (const auto& line : board) {
		for (const auto& square : line) {
			pgn += square.to_string() + ",";
		}
	}
	return pgn;
}

Colored_piece::Colored_piece(std::string pgn) {
	if (pgn.length() == 0) {
		piece = NO_PIECE;
		color = NO_COLOR;
	}
	if (pgn.length() != 2) {
		throw std::invalid_argument(
		    "PGN string must be empty or 2 chars");
	}
	switch (pgn[0]) {
	case 'w':
		color = WHITE;
		break;
	case 'b':
		color = BLACK;
		break;
	default:
		throw std::invalid_argument(
		    "PGN string must start with 'w' or 'b'");
	}
	switch (pgn[1]) {
	case 'K':
		piece = KING;
		break;
	case 'Q':
		piece = QUEEN;
		break;
	case 'R':
		piece = ROOK;
		break;
	case 'B':
		piece = BISHOP;
		break;
	case 'N':
		piece = KNIGHT;
		break;
	case 'P':
		piece = PAWN;
		break;
	default:
		throw std::invalid_argument(
		    "PGN string must end with 'K', 'Q', 'R', 'B', 'N' "
		    "or 'P'");
	}
}
std::string Colored_piece::to_utf() const {
	const bool is_white = color == WHITE;

	switch (piece) {
	case KING:
		return is_white ? "\u265A" : "\u2654";
	case QUEEN:
		return is_white ? "\u265B" : "\u2655";
	case ROOK:
		return is_white ? "\u265C" : "\u2656";
	case BISHOP:
		return is_white ? "\u265D" : "\u2657";
	case KNIGHT:
		return is_white ? "\u265E" : "\u2658";
	case PAWN:
		return is_white ? "\u265F" : "\u2659";
	case NO_PIECE:
		return " ";
	default:
		assert(false);
	}
}
std::string Colored_piece::to_string() const {
	const bool is_white = color == WHITE;

	switch (piece) {
	case KING:
		return is_white ? "wK" : "bK";
	case QUEEN:
		return is_white ? "wQ" : "bQ";
	case ROOK:
		return is_white ? "wR" : "bR";
	case BISHOP:
		return is_white ? "wB" : "bB";
	case KNIGHT:
		return is_white ? "wN" : "bN";
	case PAWN:
		return is_white ? "wP" : "bP";
	case NO_PIECE:
		return "";
	default:
		assert(false);
	}
}
