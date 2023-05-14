#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <limits>

#include "player/player.hpp"

#define INT_MAX std::numeric_limits<int>::max()

class Socket {
	int fd = -1;
	sockaddr_in address;

       public:
	Socket()                          = default;
	Socket(const Socket &)            = delete;
	Socket &operator=(const Socket &) = delete;
	Socket &operator=(Socket &&)      = delete;

	Socket(Socket &&);

	Socket(int fd, sockaddr_in address) : fd(fd), address(address) {}
	~Socket() { ::close(fd); }

	int get_fd() { return fd; }
	sockaddr_in get_address() { return address; }

	size_t send(const void *buf, size_t len, int flags = 0);
	size_t recv(void *buf, size_t len, int timeout_ms = INT_MAX,
		    int flags = 0);

	void close();
};

class Player_remote : public Player {
	Socket socket;
	bool is_white;
	bool is_connected = false;

       public:
	Player_remote(Socket socket) : socket(std::move(socket)) {
		is_connected = true;
	};
	~Player_remote() = default;

	void start_new_game(bool is_white) override;
	Player_move play(Chessboard chessboard) override;
	Player_move invalid_move(Chessboard chessboard) override;
	void end() override;

	void close() {
		socket.close();
		is_connected = false;
	};
};
