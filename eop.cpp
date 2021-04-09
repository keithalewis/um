// eop.cpp - European option pricing
// Every positive random variable F can be parameterized by
// F = f exp(s X - kappa(s)), where E[X] = 0, Var(X) = 1 and kappa(s) = log E[exp(s X)]
// Note f = E[F] and s^2 = Var(log F).
// F <= k iff X <= x(k) = (log(k/f) + kappa(s))/s
// k = f exp(s x - kappa(s)), dk/dx = k s
// digital d = E[1(F <= k)] = P(F <= k)
// put p = E[max{k - F, 0}] = E[(k - F)1(F <= k) = k P(F <= k) - f P_s(F <= k),
// where dP_s/dP = exp(s X - kappa(s)) is the Esscher transform

#include <cmath>
#include <cstddef>

// concept model ...

template<class X = double, class S = double>
struct normal {
	using xtype = X;
	using stype = S;
	// cumulative distribution function
	// D_x^n P_s(X <= x)
	static X cdf(const X& x, const S& s = 0, size_t n = 0)
	{
		return x*s + n;
	}
	// Esscher distribution function
	// D_s^n P_s(X <= x)
	static X edf(const X& x, const S& s, size_t n = 1)
	{
		return x*s;
	}
	// cumulant generating function
	// D_s^n log E[exp(s X)]
	static X cgf(const S& s, size_t n = 0)
	{
		return s + n;
	}
};

template<class K, class F, class S>
inline auto moneyness(const K& k, const F& f, const S& s, const S& ks)
{
	return (log(k/f) + ks)/s;
}

template<class F = double, class S = double>
struct instrument {
	F f;
	S s;
};

// 1(F_t <= k)
template<class K = double, class T = double>
struct digital_put {
	K k;
	T t;
};

// max{k - F_t, 0}
template<class K = double, class T = double>
struct put {
	K k;
	T t;
};

template<class Model, class Instrument, class Option>
inline auto value(const Model&, const Instrument&, const Option&);


// P(F <= k) = P(X <= x)
template<class M, class I, class X = typename M::xtype, class S = typename M::stype>
inline auto value(const M& m, const I& i, const digital_put<X,S>& o)
{
	auto srt = i.s * sqrt(o.t);
	auto x = moneyness(o.k, i.f, srt, m.cgf(srt));

	return m.cdf(x);
}

extern "C" inline double value_black_digital_put(double f, double s, double k, double t)
{
	return value(normal{}, instrument<double,double>{f, s}, digital_put<double,double>{k, t});
}

// Psi(k) = P(F <= k) = P(X <= x(k)) = Phi(x(k))
// psi(k) = Psi'(k) = phi(x(k)) dx/dk = phi(x(k)) / dk/dx = phi(x(k))/ks
// D_f E[1(F <= k)] = E[-delta_k(F) D_f F] = -f E_s[delta_k(F)] = -f phi(x(k))/ks
template<class M, class I, class X = typename M::xtype, class S = typename M::stype>
inline auto delta(const M& m, const I& i, const digital_put<X,S>& o)
{
	auto srt = i.s * sqrt(o.t);
	auto x = moneyness(o.k, i.f, srt, m.cgf(srt));

	return -i.f * m.cdf(x, 1)/(i.s * o.k);
}

int main()
{
	normal N;
	{
		instrument<> S = {100., 0.2};
		digital_put<> d = {100., 0.25};
		auto v = value(N, S, d);
		v = v + 1;
	}
	{
		auto v = value(N, instrument<>{100., 0.2}, digital_put<>{100., 0.25});
		v = v + 1;
	}

	return 0;
}
