// CPPFLAGS = -g -Wall -std=c++17
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <tuple>

typedef double Time;
using Omega = std::function<double(Time)>; // sample path

class Bond {
	double r;
public:
	Bond(double r)
		: r(r)
	{ }
	auto operator()(Time t) const
	{
		return [*this,t](const Omega&) { return exp(-r*t); };
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
	auto operator()(Time t) const
	{
		return [*this, t](const Omega& omega) {
			return s * exp(v*omega(t) - v*v*t/2);
		};
	}
};
int test_Stock()
{
	{
		Stock s(100, 0.1);
		auto omega = [](Time t) { return sqrt(t); };
		double t = 1;
		double st = s(t)(omega);
		double ot = omega(t);
		assert (st == s.s * exp(s.v*ot - s.v*s.v*t/2));
	}

	return 0;
}
int test_Stock_ = test_Stock();

template<class U> // underlying
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
	// expiration, (omega) => payoff
	auto operator*() const
	{
		return std::tuple(t, [*this](const Omega& omega) {
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
			auto omega = [o](Time) { return o; };

			double s0 = s(t)(omega);
			double p0 = a(omega);
			assert(p0 == std::max(p.k - s0, 0.));
		}
	}

	return 0;
}
int test_Put_ = test_Put();


// V_t = E_t[sum_{u > t} A_u D_u]
template<class Instrument, class Deflator>
std::function<std::function<double(const Omega&)>(Time)> value(const Instrument& A, const Deflator& D)
{
	return [&A, &D](Time t) {
		return [t, &A, &D](const Omega& omega) {
			Instrument A_(A);
			while (A_) {
				const auto& [u, a] = *A_;
				if (u <= t) {
					++A_;
					continue;
				}

				return a(omega) * D(u)(omega) + value(++A_, D)(t)(omega);
			}

			return 0.;
		};
	};
}
int test_value()
{
	{
		Stock s(100, 0.1);
		Put p(s, 100, 0.25);
		double r = 0.01;
		Bond b(r);

		auto v = value(p, b);
		double t = 0;
		auto vt = v(t);

		for (double o : {-0.1, 0., 0.1}) {
			auto omega = [o](Time) { return o; };
			double vt0 = vt(omega);
			double st0 = s(p.t)(omega);
			assert(vt0 == b(p.t)(omega)*std::max(p.k - st0, 0.));
		}

		auto vt0 = v(p.t + 1); // past expiration
		assert (vt0(0) == 0);
	}

	return 0;
}
int test_value_ = test_value();

int main()
{
	return 0;
}
