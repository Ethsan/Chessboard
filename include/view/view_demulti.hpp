#pragma once
#include <memory>

#include "view/view.hpp"

template <class U, class V>
class View_demulti : public View {
	static_assert(std::is_base_of<View, U>::value,
		      "U is not derived from View");
	static_assert(std::is_base_of<View, V>::value,
		      "V is not derived from View");

       private:
	std::unique_ptr<U> u = std::make_unique<U>();
	std::unique_ptr<V> v = std::make_unique<V>();

       public:
	View_demulti()                                = default;
	View_demulti(const View_demulti &)            = delete;
	View_demulti(View_demulti &&)                 = delete;
	View_demulti &operator=(const View_demulti &) = delete;
	View_demulti &operator=(View_demulti &&)      = delete;

	void start_new_game(Chessboard chessboard) override {
		u->start_new_game(chessboard);
		v->start_new_game(chessboard);
	}

	void update(Chessboard chessboard) override {
		u->update(chessboard);
		v->update(chessboard);
	}

	void end() override {
		u->end();
		v->end();
	}
};
