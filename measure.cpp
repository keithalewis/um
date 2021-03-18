// measure.cpp - integrate a stochastic process over a measure
#include <cassert>
#include <cmath>
#include <compare>
#include <limits>
#include <tuple>

// Random walk atom with W_n = k
class RW {
public:
	int k, n;
	RW(int k, int n)
		: k(k), n(n)
	{ 
		assert(n >= 0);
		assert(-n <= k and k <= n);
		assert((n - k) % 2 == 0);
	}
	//auto operator<=>(const RW&) const = default;
	auto operator==(const RW& o) const 
	{
		return k == o.k and n == o.n;
	}

	double operator()(int t) const
	{
		return t == n ? k : std::numeric_limits<double>::quiet_NaN();
	}
	class Atoms {
		int _k, k_;
		int t;
	public:
		Atoms(const RW& a, int t)
			: _k(a.k - t + a.n), k_(a.k + t - a.n), t(t) 
		{
			assert(t >= a.n);
		}
		explicit operator bool() const
		{
			return _k <= k_;
		}
		RW operator*() const
		{
			return RW(_k, t);
		}
		Atoms& operator++()
		{
			_k += 2;

			return *this;
		}
	};
	// atoms at time t containing this
	Atoms operator[](int t) const
	{
		return Atoms(*this, t);
	}
	// C(n, k)/2^n
	double P() const
	{
		double p = 1;
		int k_ = (n + k)/2;

		if (k_ > n/2) {
			k_ = n - k_;
		}
		for (int i = 0; i < k_; ++i) {
			p *= n - i;
			p /= i + 1;
		}

		return ldexp(p, -n);
	}
};

double P(const RW& o)
{
	return o.P();
}

int test_rw()
{
	{
		RW o(2,4);
		auto o2(o);
		o = o2;
		assert(P(o) == 4./16);
	}

	return 0;
}
int test_rw_ = test_rw();

// {t, o} where T = t on o
class barrier {
	int h;
	int t;
public:
	typedef std::tuple<int,RW> value_type;

	barrier(int h)
		: h(h), t(h)
	{ }
	explicit operator bool() const
	{
		return t < 100; // arbitrary
	}
	value_type operator*() const
	{
		return {t, RW(h, t)};
	}
	barrier& operator++()
	{
		t += 2;

		return *this;
	}
};
int test_barrier()
{
	{
		barrier H(3);
		assert(H);
		auto [t, o] = *H;
		assert(t == 3);
		assert(o == RW(3,3));

		++H;
		assert(H);
		std::tie(t, o) = *H;
		assert(t == 5);
		assert(o == RW(3,5));
	}

	return 0;
}
int test_barrier_ = test_barrier();

// return (x(t), omega)
template<class T, class X, class O = RW>
class integrand {
	T t;
	const X& x;
public:
	integrand(const T& t, const X& x)
		: t(t), x(x)
	{ }
	explicit operator bool() const
	{
		return !!t;
	}
	auto operator*() const
	{
		const auto& [t0, o] = *t;

		return std::tuple(x(t0), o);
	}
	integrand& operator++()
	{
		++t;

		return *this;
	}
};

int test_integrand()
{
	{
		auto X = [](int t) { return [](const RW& o) { return o.k; };};
		int h = 2;
		barrier H(h);
		integrand I(H, X);

		assert(I);
		{
			auto [x, o] = *I;
			assert(x(o) == h);
			assert(o == RW(h, h));
		}

		assert(++I);
		{
			auto [x, o] = *I;
			assert(x(o) == h);
			assert(o == RW(h, h + 2));
		}
	}

	return 0;
}
int test_integrand_ = test_integrand();

// evaluate \int x_t dm for stopping time t
template<class T, class X, class M>
double integral(const T& t, const X& x, const M& m)
{
	double I = 0;

	for (integrand I_(t, x); I_; ++I_) {
		auto [x_, o] = *I_;
		I += x_(o) * m(o);
	}

	return I;
}
int test_integral()
{
	{
		auto X = [](int t) { return [](const RW& o) { return o.k; };};
		int h = 2;
		barrier H(h);
		integrand I(H, X);
		double i;
		i = integral(H, X, P);
		i = i;
		i = i + 1;
	}

	return 0;
}
int test_integral_ = test_integral();

#if 0

template<class I, class S = double>
S sum(I i)
{
	S s = 0;

	while(i) {
		s += *i;
		++i;
	}

	return s;
}

#endif // 0

int main()
{
	return 0;
}
