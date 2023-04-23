#pragma once

class Player {
       public:
	Player();
	Player(const Player &) = default;
	Player(Player &&)      = delete;
	virtual ~Player()      = 0;
};
