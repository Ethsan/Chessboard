#pragma once

#include "player/player.hpp"

/**
 * @brief Simple player that ask for input in the terminal
 */
class Player_tui : public Player {
	bool parse_move(std::string input, board::Move &move);
	bool is_white;
	bool is_started = false;

       public:
	Player_tui()                              = default;
	Player_tui(const Player_tui &)            = delete;
	Player_tui(Player_tui &&)                 = delete;
	Player_tui &operator=(const Player_tui &) = delete;
	Player_tui &operator=(Player_tui &&)      = delete;
	~Player_tui() override                    = default;

	/**
	 * @brief Do nothing
	 *
	 * @param is_white Boolean
	 */
	void start_new_game(bool is_white) override;
	/**
	 * @brief Ask for input in the terminal
	 *
	 * @param chessboard Chessboard Object
	 * @return Player_move The action and the move that the player want to
	 * do
	 */
	Player_move play(Chessboard chessboard) override;
	/**
	 * @brief Print an error message in the terminal and ask for a new input
	 *
	 * @param chessboard Chessboard Object
	 * @return The action and the move that the player want to do
	 */
	Player_move invalid_move(Chessboard chessboard) override;
	/**
	 * @brief Do nothing
	 *
	 */
	void end() override;
};
