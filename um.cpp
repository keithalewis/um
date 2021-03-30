// CPPFLAGS = -g -Wall -std=c++17
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <tuple>

//typedef double Time;
//using Omega = std::function<double(Time)>; // sample path

class Binomial {
public:
	class Atom {
		int n, k; // W_n = k
	public:
		typedef int time_type;
		typedef double prob_type;
		Atom(int n, int k)
			: n(n), k(k)
		{
			assert(n >= 0);
			assert(-n <= k);
			assert(k <= n);
			assert((n - k) % 2 == 0);
		}
		Atom(const Atom&) = default;
		Atom& operator=(const Atom&) = default;
		~Atom()
		{ }

		// level
		int operator()(int t) const
		{
			assert(t == n);

			return k;
		}
		int time() const
		{
			return n;
		}
		prob_type prob() const
		{
			prob_type p = 1;
			int k_ = (n + k) / 2;

			if (k_ > n / 2) {
				k_ = n - k_;
			}
			for (int i = 0; i < k_; ++i) {
				// p /= 2;
				p *= n - i;
				p /= i + 1;
			}

			return ldexp(p, -n /* + k_*/);
		}
	};
};

template<class T>
inline T Time(const Binomial::Atom& a)
{
	return static_cast<T>(a.time());
}

class Bond {
	double r; // continuously compounded rate
public:
	Bond(double r)
		: r(r)
	{ }
	template<class Time>
	auto operator()(Time t) const
	{
		return [*this,t](const auto&) { return exp(-r*t); };
	}
};
int test_Bond()
{
	{
		Bond D(0);
		assert(D(0)(0) == 1);
		assert(D(1)(0) == 1);
	}
	{
		Bond D(0.1);
		assert(D(0)(0) == 1);
		assert(D(2)(0) == exp(-0.1*2));
	}

	return 0;
}
int test_Bond_ = test_Bond();

class Stock {
public:
	double s;
	double v;
	Stock(double s, double v)
		: s(s), v(v)
	{ }
	template<class Time>
	auto operator()(Time t) const
	{
		return [*this, t](const auto& omega) {
			return s * exp(v*omega(t) - v*v*t/2);
		};
	}
};
int test_Stock()
{
	{
		Stock s(100, 0.1);
		auto omega = [](double t) { return sqrt(t); };
		double t = 1;
		double st = s(t)(omega);
		double ot = omega(t);
		assert (st == s.s * exp(s.v*ot - s.v*s.v*t/2));
	}

	return 0;
}
int test_Stock_ = test_Stock();

template<class U, class Time = double> // underlying
class Put {
public:
	const U& u;
	double k;
	Time t;
	Put(const U& u, double k, Time t)
		: u(u), k(k), t(t)
	{ }
	explicit operator bool() const
	{
		return t != 0;
	}
	// [expiration, (omega) => payoff]
	auto operator*() const
	{
		return std::tuple(t, [*this](const auto& omega) {
			return std::max(k - u(t)(omega), 0.);
		});
	}
	Put& operator++()
	{
		t = 0;

		return *this;
	}
};
int test_Put()
{
	{
		Stock s(100, 0.1);
		Put p(s, 100, 0.25);
		assert(p);
		assert(p.k == 100);
		assert(p.t == 0.25);
		++p;
		assert(!p);
	}
	{
		Stock s(100, 0.1);
		Put p(s, 100, 0.25);
		const auto& [t, a] = *p;
		assert(t == p.t);

		for (double o : {0.0, 0.1, -0.1}) {
			auto omega = [o](double) { return o; };

			double s0 = s(t)(omega);
			double p0 = a(omega);
			assert(p0 == std::max(p.k - s0, 0.));
		}
	}

	return 0;
}
int test_Put_ = test_Put();


// V_t = E_t[sum_{u > t} A_u D_u]
template<class Instrument, class Deflator, class Time = double>
auto value(const Instrument& A, const Deflator& D)
{
	return [&A, &D](Time t) {
		return [t, &A, &D](const auto& omega) {
			double V = 0;
			Instrument A_(A);
			while (A_) {
				const auto& [u, a] = *A_;

				if (u <= t) {
					++A_;
					continue;
				}

				V += a(omega) * D(u)(omega);
				++A_;
			}

			return V;
		};
	};
}
int test_value()
{
	{
		Stock s(100, 0.1);
		Put p(s, 100, 0.25);
		double r = 0.01;
		Bond D(r);

		auto v = value(p, D);
		double t = 0;
		auto vt = v(t);

		for (double o : {-0.1, 0., 0.1}) {
			auto omega = [o](double) { return o; };
			double vt0 = vt(omega);
			double st0 = s(p.t)(omega);
			assert(vt0 == std::max(p.k - st0, 0.) * D(p.t)(omega));
		}

		auto vt0 = v(p.t + 1); // past expiration
		assert (vt0([](double) { return 0; }) == 0);
	}

	return 0;
}
int test_value_ = test_value();
#if 0

// zero coupon bond D_t(u) = E_t[D_u]/D_t
template<class Deflator>
class Zero {
public:
	const Deflator& D;
	Time u;
	Zero(const Deflator& D, Time u)
		: D(D), u(u)
	{ }
	auto operator()(Time t) const
	{
		return [*this,t](const auto& omega) {
			return D(u)(omega)/D(t)(omega);
		};
	}
};

// F_t(u, v) = (D_t(u)/D_t(v) - 1)/dcf
template<class Deflator>
class Forward {
public:
	const Deflator& D;
	Time u, v;
	double dcf;
	Forward(const Deflator& D, Time u, Time v, double dcf = 0)
		: D(D), u(u), v(v), dcf(dcf ? dcf : v - u)
	{ }
	auto operator()(Time t) const
	{
		return [*this,t](const auto& omega) {
			Zero Du(D, u);
			Zero Dv(D, v);

			return (Du(t)(omega)/Dv(t)(omega) - 1)/dcf;
		};
	}
};

class FixedLeg {
public:
	double c; // coupon
	int freq; // should really be monthly, quarterly, ...
	Time t;
	FixedLeg(double c, int freq, Time eff = 0)
		: c(c), freq(freq), t(eff + 1./freq)
	{ }
	explicit operator bool() const
	{
		return true;
	}
	auto operator*() const
	{
		return std::tuple(t, [*this](const auto&) {
			return c/freq;
		});
	}
	FixedLeg& operator++()
	{
		t += 1./freq;

		return *this;
	}
};

template<class Deflator>
class FloatLeg {
	const Deflator& D;
	int freq;
	Time u, v; // current period
	FloatLeg(const Deflator& D, int freq, Time eff = 0)
		: D(D), freq(freq), u(eff), v(eff + 1./freq)
	{ }
	explicit operator bool() const
	{
		return true;
	}
	auto operator*() const
	{
		return std::tuple(v, [*this](const auto& omega) {
			Forward F(D, u, v);

			return F(u)(omega);
		});
	}
	FloatLeg& operator++()
	{
		u = v;
		v += 1./freq;

		return *this;
	}
};

template<class Deflator>
class Swap {
	FixedLeg fixed_;
	FloatLeg<Deflator> float_;
	int freq;
	Time eff, mat, cur;
	Swap(const Deflator& D, double c, int freq, Time mat, Time eff = 0)
		: fixed_(c, freq, eff), float_(D, freq, eff), freq(freq), eff(eff), mat(mat), cur(eff + 1./freq)
	{ }
	explicit operator bool() const
	{
		return cur <= mat;
	}
	auto operator*() const
	{
		return std::tuple(cur, [*this](const auto& omega) {
			return std::get<1>(*fixed_)(omega) - std::get<1>(*float_)(omega);
		});
	}
	Swap& operator++()
	{
		++fixed_;
		++float_;
		cur += 1./freq;

		return *this;
	}
};
#endif // 0

int main()
{
	return 0;
}
