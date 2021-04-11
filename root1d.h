// root1d.h - one dimensional root finding
#pragma once
#include <algorithm>
#include <functional>
#include <iterator>

namespace polyfin {

	template<class X, class Y>
	inline constexpr auto relabs(const X& x, const Y& y)
	{
		return fabs(x)/std::max(Y(1), fabs(y));
	}
	template<class X, class Y>
	inline constexpr auto relabs(const std::pair<X,Y>& p)
	{
		return relabs(p.first, p.second);
	}

	// until(i, [](auto i) {return relabs(*i) < eps;});
	// secant s(...);
	// auto t = relabs(s) < eps;
	// [x,y] = *any(t);

	template<class X, class Y>
	class secant {
		const std::function<Y(X)>& f;
		X x0, x1;
		Y y0, y1;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<X,Y>;

		secant(const std::function<Y(X)>& f, double x0, double x1)
			: f(f), x0(x0), x1(x1), y0(f(x0)), y1(f(x1))
		{ }

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
			auto x = (x0*y1 - x1*y0)/(y1 - y0);
			x0 = x1;
			y0 = y1;
			x1 = x;
			y1 = f(x);

			return *this;
		}
		secant operator++(int)
		{
			secant s_{*this};
			operator++();

			return s_;
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
