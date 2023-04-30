#include <memory>
#include <type_traits>

#include "logic/chessboard.hpp"
#include "view/view.hpp"

template <class U, class V>
class Demultiplexer : public View {
	static_assert(std::is_base_of<View, U>::value,
		      "U is not derived from View");
	static_assert(std::is_base_of<View, V>::value,
		      "V is not derived from View");

       private:
	std::unique_ptr<U> u = std::make_unique<U>();
	std::unique_ptr<V> v = std::make_unique<V>();

       public:
	Demultiplexer()                                 = default;
	Demultiplexer(const Demultiplexer &)            = delete;
	Demultiplexer(Demultiplexer &&)                 = delete;
	Demultiplexer &operator=(const Demultiplexer &) = delete;
	Demultiplexer &operator=(Demultiplexer &&)      = delete;

	void start_new_game(cboard::Chessboard chessboard) override {
		u->start_new_game(chessboard);
		v->start_new_game(chessboard);
	}

	void update(cboard::Chessboard chessboard) override {
		u->update(chessboard);
		v->update(chessboard);
	}

	void end() override {
		u->end();
		v->end();
	}
};
