// root1d.h - one dimensional root finding
#pragma once
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include "fms_iterable.h"

namespace fms::root1d {

	template<class X>
	inline constexpr X epsilon() { return std::numeric_limits<X>::epsilon(); };

	template<class X>
	inline constexpr X minimum() { return std::numeric_limits<X>::(min)(); };

	template<class X>
	inline constexpr X maximum() { return std::numeric_limits<X>::(max)(); };

	template<class X>
	inline constexpr bool nearly_equal(const X& a, const X& b,
		const X& rel = 8*epsilon<X>(), const X& abs = minimum<X>())
	{
		if (a == b) return true;

		return std::fabs(a - b) <= std::max(abs, rel * (std::fabs(a) + std::fabs(b)));
	}
	
	/*
	template<fms::iterable I, class P> 
	inline I solve(I i, const P& p)
	{
		return back(i, p);
	}
	*/

	// until(i, [](auto i) {return relabs(*i) < eps;});
	// secant s(...);
	// auto t = relabs(s) < eps;
	// [x,y] = *any(t);

	template<class X, class Y>
	struct secant {
		const std::function<Y(X)>& f;
		X x0, x1;
		Y y0, y1;
		decltype(Y / X) m_, m; // last and current secant slope
	public:
		using iterator_category = std::input_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = std::pair<X, Y>;
		using pointer = value_type*;
		using reference = value_type&;

		secant(const std::function<Y(X)>& f, X x0, X x1)
			: f(f), x0(x0), x1(x1), y0(f(x0)), y1(f(x1)), m((y1 - y0)/(x1 - x0)), m_(m)
		{ }

		bool operator==(const secant& s) const
		{
			return x0 == s.x0 and x1 == s.x1 and y0 == s.y0 and y1 == s.y1
				and &f == &s.f;
		}

		explicit operator bool() const
		{
			return y1 != Y(0);
		}
		value_type operator*() const
		{
			return value_type(x1, y1);
		}
		secant& operator++()
		{
			bool bounded = y0 * y1 < 0;

			X x_ = x1 - y1 / m;
			Y y_ = f(x_);

			x0 = x1;
			y0 = y1;
			x1 = x_;
			y1 = y_;
			m_ = m;
			m = (y1 - y0) / (x1 - x0);

			if (bounded and std::fabs(m) < std::fabs(m_)) {
				m = m_; // don't zoom off
			}

			return *this;
		}
		secant operator++(int)
		{
			secant s_{*this};
			operator++();

			return s_;
		}

		static bool nearly_zero(const secant& s) const
		{
			return nearly_equal(s.x0, s.x1) and nearly_equal(x.y1, 0);
		}
		value_type solve()
		{
			return back(until(*this, nearly_zero));
		}
#ifdef _DEBUG
		static int test()
		{
			{
				auto f = [](X x) { return x*x - 2; };
				secant s(f, 1, 2);
				assert(s);

				X x;
				Y y;
				std::tie(x,y) = *s;
				assert(x == 2 and y == f(x));

				++s;
				std::tie(x,y) = *s;
				assert(x == 4./3 and y == f(x));
			}

			return 0;
		}
#endif // _DEBUG
	};


} // namespace polyfin
