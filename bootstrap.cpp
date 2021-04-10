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
	I e;
	P p;
public:
	using value_type = I;

	sequence(const I& b, const I& e, const P& p)
		: b(b), e(e), p(p)
	{ }
	~sequence()
	{ }

	bool operator==(const sequence& s) const
	{
		return b == s.b and e == s.e and p == s.p;
	}
	bool operator!=(const sequence& s) const
	{
		return !operator==(s);
	}

	sequence begin() const
	{
		return *this;
	}
	sequence end() const
	{
		return sequence(e + p, e + p, p);
	}

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
		if (b <= e) {
			b += p;
		}
		else {
			b = e = e + p;
		}

		return *this;
	}
	sequence operator++(int)
	{
		sequence tmp{*this};
		operator++();

		return tmp;
	}
};

using namespace std::chrono;

int test_sequence()
{
	{
		int b = 0;
		int e = 5;
		sequence s(b, e, 1);
		int i = b;
		while (s) {
			assert(i == *s);
			++i;
			++s;
		}
		assert(i == e + 1);
	}
	{
		int b = 0;
		int e = 5;
		sequence s(b, e, 1);
		int i = b;
		while (s) {
			assert(i == *s);
			i++;
			s++;
		}
		assert(i == e + 1);
	}

	return 0;
}
int test_sequence_ = test_sequence();

// f(t) = x[i], t[i-1] < t <= t[i]
template<class T, class X>
class pwflat {
	size_t n;
	const T* t;
	const X* x;
public:
	using iterator_category = std::input_iterator_tag;
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
	pwflat(const pwflat&) = default;
	pwflat& operator=(const pwflat&) = default;
	~pwflat()
	{ }

	bool operator==(const pwflat& f) const
	{
		return n == f.n and t == f.t and x == f.x;
	}
	bool operator!=(const pwflat& f) const
	{
		return !operator==(f);
	}

	pwflat begin() const
	{
		return *this;
	}
	pwflat end() const
	{
		return pwflat{};
	}

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
		else {
			t = nullptr;
			x = nullptr;
		}

		return *this;
	}
	pwflat operator++(int)
	{
		pwflat tmp{*this};
		operator++();

		return tmp;
	}
};
int test_pwflat()
{
	{
		int t[] = {1,2,3};
		double x[] = {.1,.2,.3};
		pwflat f(3, t, x);
		assert(f);
		pwflat f2{f};
		assert(f2 == f);
		f = f2;
		assert(!(f != f2));

		int i = 0;
		{
			assert(f);
			auto [ti, xi] = *f;
			assert(ti == t[i] and xi == x[i]);
		}
		++i;
		++f;
		{
			assert(f);
			auto [ti, xi] = *f;
			assert(ti == t[i] and xi == x[i]);
		}
		++i;
		++f;
		{
			assert(f);
			auto [ti, xi] = *f;
			assert(ti == t[i] and xi == x[i]);
		}
		++f;
		assert(!f);
	}

	return 0;
}
int test_pwflat_ = test_pwflat();

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

// present value of cash flows up to _u and discount to last cash flow
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

	return std::pair(pv, D);
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
