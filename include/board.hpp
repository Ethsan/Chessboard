#pragma once

#include <array>

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

struct Colored_piece {
	Piece piece;
	Color color;
};

enum Row : int {
	ROW_A,
	ROW_B,
	ROW_C,
	ROW_D,
	ROW_E,
	ROW_F,
	ROW_G,
	ROW_H,
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

struct Square {
	Line line;
	Row row;
};

typedef std::array<std::array<Colored_piece, 8>, 8> Board;

}  // namespace board
