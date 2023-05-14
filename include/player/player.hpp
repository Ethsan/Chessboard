#pragma once

#include "logic/chessboard.hpp"

/**
 * @brief Enum that represent the possible actions that a player can perform
 */
enum Action { PLAY, RESIGN, DRAW, END };

/**
 * @brief Struct that represent the action and in the case continue to play the
 * chess move it want to do
 */
struct Player_move {
	Action action;
	board::Move move;
};

/**
 * @brief Class that represent a player
 */
class Player {
       public:
	Player()          = default;
	virtual ~Player() = default;

	/**
	 * @brief Method called by the controller when the game start
	 *
	 * @param is_white True if the player is white, false otherwise
	 */
	virtual void start_new_game(bool is_white) = 0;
	/**
	 * @brief Method called by the controller after each move
	 *
	 * @param chessboard Chessboard Object
	 * @return Player_move The action and the move that the player want to
	 * do
	 */
	virtual Player_move play(Chessboard chessboard) = 0;
	/**
	 * @brief Method called by the controller when the player try to do an
	 * invalid move
	 *
	 * @param chessboard Chessboard Object
	 * @return Player_move The action and the move that the player want to
	 * do
	 */
	virtual Player_move invalid_move(Chessboard chessboard) = 0;
	/**
	 * @brief Method called by the controller when the game ends
	 *
	 */
	virtual void end() = 0;
};
