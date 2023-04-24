#include "logic/chessboard.hpp"
#include "view/tui.hpp"

using namespace board;
int main(int argc, char *argv[])
{
	(void )argc;
	(void) argv;
	cboard::Chessboard board;
	View *tui = new Tui(board);
	assert(board.move(Square("a2"), Square("a4")));
	tui->update(board);
	board.move(Square("b7"), Square("b5"));
	tui->update(board);
	board.move(Square("a4"), Square("b5"));
	tui->update(board);
	tui->end();

	
	return 0;
}
