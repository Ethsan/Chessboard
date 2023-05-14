#pragma once

#include <random>

#include "player/player.hpp"

class Player_random : public Player {
	bool is_white;
	bool is_started = false;
	std::default_random_engine generator;

       public:
	Player_random()                                 = default;
	Player_random(const Player_random &)            = delete;
	Player_random(Player_random &&)                 = delete;
	Player_random &operator=(const Player_random &) = delete;
	Player_random &operator=(Player_random &&)      = delete;
	~Player_random() override                       = default;

	/**
	 * @brief Do nothing
	 *
	 * @param is_white Boolean
	 */
	void start_new_game(bool is_white) override;
	/**
	 * @brief Get all the possible moves and choose one randomly
	 *
	 * @param chessboard Chessboard Object
	 * @return Player_move The action and the move that the player want to
	 * do
	 */
	Player_move play(Chessboard chessboard) override;
	/**
	 * @brief Simply crash should not happen
	 *
	 * @param chessboard Chessboard Object
	 * @return Doesn't return
	 */
	Player_move invalid_move(Chessboard chessboard) override;
	/**
	 * @brief Do nothing
	 *
	 */
	void end() override;
};
