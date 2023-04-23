#include "bitboard.hpp"

class Player {
       public:
	Player();
	Player(const Player &)            = default;
	Player(Player &&)                 = delete;
	Player &operator=(Player &&)      = delete;
	virtual ~Player()                 = 0;

};
