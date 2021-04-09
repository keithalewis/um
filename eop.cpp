// eop.cpp - European option pricing
// Every positive random variable F can be parameterized by
// f = E[F], s^2 = Var(log F), and X where E[X] = 0, Var(X) = 1.
// F = f exp(s X - kappa(s)), kappa(s) = log E[exp(s X)]
// F <= k iff X <= x(k) = (log(k/f) + kappa(s))/s
// digital d = E[1(F <= k)] = P(F <= k)
// put p = E[max{k - F, 0}] = k P(F <= k) - f P_s(F <= k)
// dP_s/dP = exp(s X - kappa(s))

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

/*
template<class K, class F, class S>
inline X moneyness(const K& k, const F& f, const S& s)
{
	return (log(k/f) + kappa(s))/s;
}
*/

// 1(F_t <= k)
template<class K, class T>
struct digital_put {
	K k;
	T t;
};

// max{k - F_t, 0}
template<class K, class T>
struct put {
	K k;
	T t;
};

template<class M, class O, class X = typename M::xtype, class S = typename M::stype>
inline X value(const M& m, const O& o);

template<class M>
inline typename M::xtype value<M>(const M& m, const digital_put& o)
{
	return m.cdf(o.k);
}

int main()
{
	return 0;
}
