#pragma once

#include "player/player.hpp"

/**
 * @brief Bot player with a depth of 4 wich is sufficient to play at a average
 * elo and fast
 */
class Player_bot : public Player {
	bool is_white;
	bool is_started = false;
	int max_depth   = 4;

       public:
	Player_bot()                              = default;
	Player_bot(const Player_bot &)            = delete;
	Player_bot(Player_bot &&)                 = delete;
	Player_bot &operator=(const Player_bot &) = delete;
	Player_bot &operator=(Player_bot &&)      = delete;
	~Player_bot() override                    = default;

	/**
	 * @brief Do nothing
	 *
	 * @param is_white Boolean
	 */
	void start_new_game(bool is_white) override;
	/**
	 * @brief Use the minimax algorithm to find the best move for the
	 * current position
	 *
	 * @param chessboard Chessboard Object
	 * @return Player_move The action and the move that the player want to
	 * do
	 */
	Player_move play(Chessboard chessboard) override;
	/**
	 * @brief Simply crash should not happen
	 *
	 * @param chessboard chessboard
	 * @return Doesn't return
	 */
	Player_move invalid_move(Chessboard chessboard) override;
	/**
	 * @brief Do nothing
	 *
	 */
	void end() override;
};
