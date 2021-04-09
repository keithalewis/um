// bootstrap.cpp - bootstrap a curve
#include <cassert>
#include <chrono>
#include <iterator>
#include <limits>
#include <iostream>

// b, b + p, ..., e
template<class I, class P>
class sequence {
	I b;
	const I& e;
	const P& p;
public:
	using value_type = decltype(b);

	sequence(const I& b, const I& e, const P& p)
		: b(b), e(e), p(p)
	{ }
	explicit operator bool()
	{
		return b <= e;
	}
	value_type operator*() const
	{
		return b;
	}
	sequence& operator++()
	{
		if (b < e) {
			b += p;
		}

		return *this;
	}
};

using namespace std::chrono;

int test_sequence()
{
	{
		auto b = system_clock::now();
		auto e = b + 2h;
		sequence i(b, e, 10min);
		while (i) {
			std::cout << system_clock::to_time_t(*i) << std::endl;
			++i;
		}
	}

	return 0;
}
int test_sequence_ = test_sequence();

// f(x) = x[0], t <= t[0], f(x) = x[i], t[i-1] < t <= t[i]
template<class T, class X>
class pwflat {
	size_t n;
	const T* t;
	const X* x;
public:
	//using iterator_category = typename ...
	using value_type = std::pair<T,X>;

	/*
	pwflat(const X x0)
		: n(1), t(&inf), x0(x0)
	{
		x = &x0;
	}
	*/
	pwflat(size_t n = 0, const T* t = nullptr, const X* x = nullptr)
		: n(n), t(t), x(x)
	{ }
	explicit operator bool() const
	{
		return n != 0;
	}
	value_type operator*() const
	{
		return std::pair(*t, *x);
	}
	pwflat& operator++()
	{
		if (n > 0) {
			--n;
			++t;
			++x;
		}
	}
};

template<class F, class T, class X>
inline X value(F& f, const T& t, const X& _x = std::numeric_limits<X>::quiet_NaN())
{
	while (f) {
		const auto& [s,x] = *f;
		if (t <= s) {
			return x;
		}
		++f;
	}

	return _x; // extrapolate
}

template<class F, class T, class X>
inline X integrate(F& f, const T& t, T t0 = 0)
{
	X I = 0;

	while (f) {
		const auto& [s,x] = *f;
		if (s < t0) {
			++f;
		}
		else if (s <= t) {
			I += x*(s - t0);
			t0 = s;
			++f;
		}
		else {
			I += x*(t - t0);

			break;
		}
	}

	return I;
}

// t r(t) = int_0^t f(s) ds
template<class F, class T, class X>
inline X spot(const F& f, const T& t)
{
	const auto& [s,x] = *f;

	return t <= s ? x : integrate(f, t)/t;
}

// present value of cash flows up to _u
template<class F, class I, class T, class X>
inline std::pair<X,X> present_value(F& f, I& i, const T& _u = std::numeric_limits<T>::infinity())
{
	T t0 = 0;
	X pv = 0;
	X D = 1;

	while (i) {
		const auto& [u,c] = *i;
		if (!f or u > _u) {
			break;
		}
		D *= exp(-integrate(f, u, t0));
		pv += c * D;
		t0 = u;
		++i;
	}

	return [pv, D];
}

/*
// p = pv0 + D0 * sum_{} c * exp(-f(u - _u))
template<class F, class I, class T, class X>
inline std::pair<T,X> bootstrap(F& f, const I& i, const X& p)
{
	const auto& [_pv, _D] = present_value(f, i);
	const auto& [_u, _x] = *f;
	auto pv = [f,i,p,pv0](X x_) {
		X pv_ = present_value(pwflat(x_), I(i), _u);

		return _D*pv_ - _pv - p;
	}

	return [?,root1d(pv)];
}
*/

int main()
{
	return 0;
}
