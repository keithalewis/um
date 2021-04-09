// sample.cpp
#include <cmath>

struct Normal {
	// cumulative distribution function
	// D_x^n P_s(X <= x)
	double cdf(double x, double s, size_t n = 0)
	{
		return x*s+n;
	}
	// Esscher distribution function
	// D_s P_s(X <= x)
	double edf(double x, double s)
	{
		return x*s+n;
	}
	// cumulant generating function
	// D_s^ log E[exp(s X)]
	double cgf(double s, size_t n = 0)
	{
		if (n == 0) {
			return s*s/2
		}
		else if (n == 1) {
			return s;
		}
		else if (n == 2) {
			return 1;
		}
		else {
			return 0;
		}
	}
};

// exp(sigma * Normal)
struct Lognormal {
	double sigma;
};

class GBM {
	double sigma;
public:
	using Time = double;
	using Atom = double; // M_t = a
	GBM(double sigma)
		: sigma(sigma)
	{ }
	Lognormal operator()(const Time& t) const
	{
		return Lognormal{sigma * sqrt(t)};
	}
};

class Stock {
	double s;
public:
	Stock(double s)
		: s(s)
	{ }
	/*
	template<class Model, class Time = Model::Time, class Atom = Model::Atom>
	auto price(const Model& m, const Time& t, const Atom& a)
	{
		return s*m(t, a)/m.deflator(t,a);
	}
	*/
};

/*
template<class O, class I, class M>
auto value(const O& o, const I& i, const M& m)
{
	using Time = double; // M::time
	using Atom = double; // M::atom

	return [&o,i,m](const Time& t, const Atom& a) {
	};
}

int example1()
{
	// price, dividends
	Stock S(100, fixed(quarterly, 0.01));
	Model GBM(0.2);
	Option put(100, 0.25);
	auto V = value(put, S, M);
	
	return 0;
}
*/

int main()
{
	return 0;
}
