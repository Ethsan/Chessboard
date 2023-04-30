#pragma once

#include <array>
#include <cassert>
#include <stdexcept>
#include <string>

namespace board {
enum Piece : int {
	NO_PIECE,
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
};

enum Color : int {
	NO_COLOR,
	WHITE,
	BLACK,
};

class Colored_piece {
       public:
	Piece piece;
	Color color;

	Colored_piece() : piece{NO_PIECE}, color{NO_COLOR} {};
	Colored_piece(Piece piece, Color color) : piece{piece}, color{color} {};
	Colored_piece(std::string pgn) {
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
	bool is_valid() const {
		return !((piece != NO_PIECE) xor (color != NO_COLOR));
	}
	std::string to_string() const {
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
	std::string to_pgn() const {
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
};
const Colored_piece xx = Colored_piece(NO_PIECE, NO_COLOR);

const Colored_piece wK{KING, WHITE};
const Colored_piece wQ{QUEEN, WHITE};
const Colored_piece wR{ROOK, WHITE};
const Colored_piece wB{BISHOP, WHITE};
const Colored_piece wN{KNIGHT, WHITE};
const Colored_piece wP{PAWN, WHITE};
const Colored_piece bK{KING, BLACK};
const Colored_piece bQ{QUEEN, BLACK};
const Colored_piece bR{ROOK, BLACK};
const Colored_piece bB{BISHOP, BLACK};
const Colored_piece bN{KNIGHT, BLACK};
const Colored_piece bP{PAWN, BLACK};

const Colored_piece white_pieces[] = {wK, wQ, wR, wB, wN, wP};
const Colored_piece black_pieces[] = {bK, bQ, bR, bB, bN, bP};

typedef std::array<std::array<Colored_piece, 8>, 8> Board;

inline Board from_pgn(const std::string& pgn) {
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

inline std::string to_pgn(const Board& board) {
	std::string pgn;
	for (const auto& line : board) {
		for (const auto& square : line) {
			pgn += square.to_pgn() + ",";
		}
	}
	return pgn;
}

const Board initial_board{{{wR, wN, wB, wQ, wK, wB, wN, wR},
			   {wP, wP, wP, wP, wP, wP, wP, wP},
			   {xx, xx, xx, xx, xx, xx, xx, xx},
			   {xx, xx, xx, xx, xx, xx, xx, xx},
			   {xx, xx, xx, xx, xx, xx, xx, xx},
			   {xx, xx, xx, xx, xx, xx, xx, xx},
			   {bP, bP, bP, bP, bP, bP, bP, bP},
			   {bR, bN, bB, bQ, bK, bB, bN, bR}}};

enum Column : int {
	COL_A,
	COL_B,
	COL_C,
	COL_D,
	COL_E,
	COL_F,
	COL_G,
	COL_H,
};

enum Line : int {
	LINE_1,
	LINE_2,
	LINE_3,
	LINE_4,
	LINE_5,
	LINE_6,
	LINE_7,
	LINE_8,
};

class Square {
       public:
	Line line;
	Column col;

	Square() : line{LINE_1}, col{COL_A} {};
	Square(Line line, Column col) : line{line}, col{col} {};
	Square(std::string square) {
		line = static_cast<Line>(square[1] - '1');
		col  = static_cast<Column>(square[0] - 'a');
	};
	std::string to_string() const {
		return std::string{static_cast<char>(col + 'a')} +
		       std::string{static_cast<char>(line + '1')};
	};
};

}  // namespace board
