#include <string>
#include "bitboard.hpp"
#include "chessboard.hpp"

class View {
       public:
	View();
	View(bboard::Bitboard bitboard);
	View(const View &) = default;

	virtual View& reset(bboard::Bitboard bitboard) = 0;
	virtual View& update(Chessboard board) = 0;
	virtual View& render() const = 0;
	virtual ~View() = default;
};
