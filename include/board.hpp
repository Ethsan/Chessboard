#pragma once

#include <array>
#include <cassert>
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

	constexpr Colored_piece(Piece piece, Color color)
	    : piece{piece}, color{color} {};
	constexpr Colored_piece() : piece{NO_PIECE}, color{NO_COLOR} {};
	constexpr bool is_valid() const {
		return piece != NO_PIECE xor color != NO_COLOR;
	}
	constexpr std::string to_string() const {
		assert(is_valid());
		const bool is_white = color == WHITE;

		switch (piece) {
		case KING:
			return is_white ? "\u2654" : "\u265A";
		case QUEEN:
			return is_white ? "\u2655" : "\u265B";
		case ROOK:
			return is_white ? "\u2656" : "\u265C";
		case BISHOP:
			return is_white ? "\u2657" : "\u265D";
		case KNIGHT:
			return is_white ? "\u2658" : "\u265E";
		case PAWN:
			return is_white ? "\u2659" : "\u265F";
		case NO_PIECE:
			return " ";
		default:
			assert(false);
		}
	}
	constexpr std::string to_pgn() const {
		assert(is_valid());
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

class Square {
       public:
	Line line;
	Row row;

	constexpr Square(Line line, Row row) : line{line}, row{row} {};
	constexpr Square(std::string square) {
		line = static_cast<Line>(square[1] - '1');
		row  = static_cast<Row>(square[0] - 'a');
	};
	constexpr std::string to_string() const {
		return std::string{static_cast<char>(row + 'a')} +
		       std::string{static_cast<char>(line + '1')};
	};
};

typedef std::array<std::array<Colored_piece, 8>, 8> Board;

}  // namespace board
