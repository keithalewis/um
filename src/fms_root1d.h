// root1d.h - one dimensional root finding
#pragma once
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>
#include <iterator>
#include <limits>
#include "fms_iterable.h"

namespace fms::root1d {

	template<class X>
	inline constexpr X epsilon() { return std::numeric_limits<X>::epsilon(); };

	template<class X>
	inline constexpr X minimum() { return std::numeric_limits<X>::min(); };

	template<class X>
	inline constexpr X maximum() { return std::numeric_limits<X>::max(); };

	template<class X>
	inline constexpr bool nearly_equal(const X& a, const X& b,
		const X& rel = 8*epsilon<X>(), const X& abs = minimum<X>())
	{
		if (a == b) return true;

		X diff = a - b;
		X norm = std::fabs(a) + std::fabs(b);
		norm = std::max(X(1), norm);

		return std::fabs(diff) <= std::max(abs, rel * norm);
	}
	
	template<fms::iterable I, class P> 
	inline I solve(I i, const P& p)
	{
		return upto(i, p);
	}

	// until(i, [](auto i) {return relabs(*i) < eps;});
	// secant s(...);
	// auto t = relabs(s) < eps;
	// [x,y] = *any(t);

	template<class X, class Y>
	struct secant {
		std::reference_wrapper<const std::function<Y(X)>> f;
		X x0, x1;
		Y y0, y1;
		decltype(y0 / x0) m_, m; // last and current secant slope
		X rel, abs;
	public:
		using iterator_category = std::input_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = std::pair<X, Y>;
		using pointer = value_type*;
		using reference = value_type&;

		secant(const std::function<Y(X)>& f, const X& x0, const X& x1,
			const X& rel = 8 * epsilon<X>(), const X& abs = minimum<X>())
				: f(std::cref(f)), x0(x0), x1(x1), y0(f(x0)), y1(f(x1)), 
					m((y1 - y0)/(x1 - x0)), m_(0), rel(rel), abs(abs)
		{ }
		secant(const secant&) = default;
		secant& operator=(const secant&) = default;
		~secant()
		{ }
		
		bool operator==(const secant& s) const
		{
			return /*&f == &s.f and*/ x0 == s.x0 and x1 == s.x1 
				and y0 == s.y0 and y1 == s.y1
				and m == s.m and m_ == s.m_
				and rel == s.rel and abs == s.abs;
		}

		secant begin() const
		{
			return *this;
		}
		secant end() const
		{
			secant s(f, x0, x1, rel, abs);
			s.y0 = s.y1 = 0; // phony end

			return s;
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
			
			if (!nearly_equal(x0, x1, rel, abs)) {
				m = (y1 - y0) / (x1 - x0);
			}

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

		bool nearly_zero() const
		{
			return nearly_equal(x0, x1, abs, rel) and nearly_equal(y1, Y(0), Y(abs), Y(rel));
		}
		value_type solve()
		{
			while (!nearly_zero())
				operator++();

			return operator*();
		}

#ifdef _DEBUG
		static int test()
		{
			{
				//secant s(cos, ...) does not work
				std::function<double(double)> f = [](double x) { return cos(x); };
				secant s(f , 3. / 2, 4. / 2);
				assert(s);
				secant s2{ s };
				assert(s2);
				assert(s == s2);
				s = s2;
				assert(!(s != s2));

				const auto& [x, y] = s.solve();
				assert(nearly_equal(x, M_PI_2));
			}
			{
				std::function<Y(X)> f = [](X x) { return x*x - 2.; };
				secant s(f, 1, 2);
				assert(s);

				X x;
				Y y;
				std::tie(x,y) = *s;
				assert(x == 2 and y == f(x));

				++s;
				std::tie(x,y) = *s;
				assert(nearly_equal(x, 4./3) and nearly_equal(y, f(x)));
			}
			{
				std::function<Y(X)> f = [](X x) { return x * x - 2; };
				secant s(f, 1, 2);
				const auto& [x,y] = s.solve();
				assert(nearly_equal(f(x), 0.));
			}
			{
				std::function<Y(X)> f = [](X x) { return x * x - 2; };
				auto s = counted(secant(f, 1, 2));
				while (!s.iterable().nearly_zero()) {
					++s;
				}
				assert(8 == s.count());
			}

			return 0;
		}
#endif // _DEBUG
	};


} // namespace polyfin
