#include <random>

#include "player/player.hpp"

class Player_random : public Player {
	bool is_white;
	bool is_started = false;
	std::default_random_engine generator;

       public:
	~Player_random() override = default;

	void start_new_game(bool is_white) override;
	Player_move play(Chessboard chessboard) override;
	Player_move invalid_move(Chessboard chessboard) override;
	void end() override;
};
