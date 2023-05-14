#include <iostream>
#include <memory>

#include "controller/controller.hpp"
#include "player/player_bot.hpp"
#include "player/player_random.hpp"
#include "player/player_tui.hpp"
#include "view/view_tui.hpp"

std::unique_ptr<Player> get_player(std::string player) {
	if (player == "human") {
		return std::make_unique<Player_tui>();
	} else if (player == "bot") {
		return std::make_unique<Player_bot>();
	} else if (player == "random") {
		return std::make_unique<Player_random>();
	}
	throw std::runtime_error("Invalid player type");
}

void print_usage(char *argv[]) {
	std::cout << "Usage: " << argv[0]
		  << " < -w [player type] > < -b [player_type] >" << std::endl;
	std::cout << "Player types: human, bot, random" << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc > 5) {
		print_usage(argv);
		return 1;
	}
	std::unique_ptr<Player> white;
	std::unique_ptr<Player> black;

	if (argc == 1) {
		white = std::make_unique<Player_tui>();
		black = std::make_unique<Player_tui>();
	} else if (argc == 3) {
		if (std::string(argv[1]) == "-w") {
			white = get_player(argv[2]);
			black = std::make_unique<Player_tui>();
		} else if (std::string(argv[1]) == "-b") {
			white = std::make_unique<Player_tui>();
			black = get_player(argv[2]);
		} else {
			print_usage(argv);
			return 1;
		}
	} else if (argc == 5) {
		if (std::string(argv[1]) == "-w" &&
		    std::string(argv[3]) == "-b") {
			white = get_player(argv[2]);
			black = get_player(argv[4]);
		} else if (std::string(argv[1]) == "-b" &&
			   std::string(argv[3]) == "-w") {
			white = get_player(argv[4]);
			black = get_player(argv[2]);
		} else {
			print_usage(argv);
			return 1;
		}
	}

	Controller controller(std::move(white), std::move(black),
			      std::make_unique<View_tui>());
	controller.start();
	return 0;
}
