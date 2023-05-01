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
	Colored_piece(std::string pgn);

	bool is_valid() const {
		return !((piece != NO_PIECE) xor (color != NO_COLOR));
	}

	std::string to_string() const;
	std::string to_pgn() const;
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

Board from_pgn(const std::string& pgn);
std::string to_pgn(const Board& board);

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

struct Move {
	Square from     = Square();
	Square to       = Square();
	Piece promotion = NO_PIECE;
};

}  // namespace board
