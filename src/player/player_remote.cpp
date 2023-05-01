#include "player/player_remote.hpp"

#include <errno.h>
#include <poll.h>
#include <string.h>

#include <cstdint>
#include <string>

using namespace std;
using namespace board;

Socket::Socket(Socket &&other) {
	fd       = other.fd;
	address  = other.address;
	other.fd = -1;
}

size_t Socket::send(const void *buf, size_t len, int flags) {
	size_t sent = 0;
	do {
		ssize_t ret = ::send(fd, (char *)buf + sent, len - sent, flags);
		if (ret < 0) {
			throw runtime_error("send failed" +
					    string(strerror(errno)));
		} else if (ret == 0) {
			break;
		}

		sent += ret;
	} while (sent < len);

	return sent;
}

size_t Socket::recv(void *buf, size_t len, int timeout_ms, int flags) {
	struct pollfd pollfd;
	int ret;
	pollfd.fd     = fd;
	pollfd.events = POLLIN;

	ret = poll(&pollfd, 1, timeout_ms);
	if (ret < 0) {
		throw runtime_error("poll failed" + string(strerror(errno)));
	} else if (ret == 0) {
		return 0;
	}

	size_t received = 0;
	do {
		ssize_t ret =
		    ::recv(fd, (char *)buf + received, len - received, flags);
		if (ret < 0) {
			throw runtime_error("recv failed" +
					    string(strerror(errno)));
		} else if (ret == 0) {
			break;
		}

		received += ret;
	} while (received < len);

	return received;
}

void Socket::close() {
	if (fd != -1) {
		::close(fd);
		fd = -1;
	}
}

struct trame {
	int8_t turn;
	int8_t action;
	int8_t from;
	int8_t to;
	int8_t promotion;
};

// *really* simple networking
// Idea : implement sync at start/end of a game, better management of network
// error ( timeout, disconnect )
void Player_remote::start_new_game(bool is_white) { this->is_white = is_white; }

Player_move Player_remote::play(Chessboard chessboard) {
	if (!is_connected) {
		return {END, {}};
	}
	int turn       = chessboard.get_turn_count();
	Move last_move = chessboard.get_last_move();

	struct trame trame_send, trame_recv;
	trame_send.turn      = turn;
	trame_send.action    = PLAY;
	trame_send.from      = last_move.from.col + last_move.from.line * 8;
	trame_send.to        = last_move.to.col + last_move.to.line * 8;
	trame_send.promotion = last_move.promotion;

	assert(socket.send(&trame_send, sizeof(trame_send)) ==
	       sizeof(trame_send));
	assert(socket.recv(&trame_recv, sizeof(trame_recv)) ==
	       sizeof(trame_recv));

	Player_move player_move;
	Move &move = player_move.move;

	player_move.action = static_cast<Action>(trame_recv.action);
	move.from          = Square(static_cast<Line>(trame_recv.from / 8),
				    static_cast<Column>(trame_recv.from % 8));
	move.to            = Square(static_cast<Line>(trame_recv.to / 8),
				    static_cast<Column>(trame_recv.to % 8));
	move.promotion     = static_cast<Piece>(trame_recv.promotion);

	return player_move;
}

Player_move Player_remote::invalid_move(Chessboard chessboard) {
	(void)chessboard;
	throw runtime_error("Unexpected invalid move");
}

// one player doesn't know the last other player move
// TODO : fix it
void Player_remote::end() {
	struct trame trame {
		.turn = INT8_MAX, .action = END, .from = INT8_MAX,
		.to = INT8_MAX, .promotion = INT8_MAX
	};
	assert(socket.send(&trame, sizeof(trame)) == sizeof(trame));
}
