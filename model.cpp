// model.cpp - unified model
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <tuple>


// concept Atom requires (A a) { a.time(); a.prob(); }

//  time associated with an atom
template<class Atom, class Time = typename Atom::time_type> // Atom::time_type
Time time(const Atom& a)
{
	return a.time();
}

// probability associated with an atom
template<class Atom>
double prob(const Atom& a)
{
	return a.prob();
}

// a stopping time T:Omega -> time is a collection of atoms of Omega
// with T(omega) = time(omega)
// atom A: B\subset A and time(B) = time(A) implies B = A or B empty

// E[X_T](omega)
template<class StoppingTime, class StochasticProcess, class Measure>
auto expected_value(const StoppingTime& T, const StochasticProcess& X, const Measure& P)
{
	return [&T, &X, &P](const auto& omega) {
		double E = 0;

		for (auto o = T(omega); o; ++o) {
			E += X(time(*o))(*o) * prob(*o);	
		}

		return E;
	};
}

// random walk atoms
class RW {
	int k, n;
public:
	typedef int time_type;
	// all paths with W_n = k
	RW(int k, int n)
		: k(k), n(n)
	{ 
		assert(n >= 0);
		assert(-n <= k);
		assert(k <= n);
		assert((n - k) % 2 == 0);
	}

	//auto operator<=>(const RW&) const = default;
	bool operator==(const RW& o) const 
	{
		return k == o.k and n == o.n;
	}
	bool operator!=(const RW& o) const 
	{
		return !operator==(o);
	}

	int time() const
	{
		return n;
	}
	// level
	int operator()(int t) const
	{
		assert(t == n);

		return k;
	}

	RW& operator++()
	{
		if (k < n) {
			k += 2;
		}

		return *this;
	}

	// C(n, k)/2^n
	double prob() const
	{
		double p = 1;
		int k_ = (n + k)/2;

		if (k_ > n/2) {
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

class ConstantStoppingTime {
	int T;
public:
	ConstantStoppingTime(int T)
		: T(T)
	{ }
	struct Atoms {
		typedef std::forward_iterator_tag iterator_category;
		typedef RW value_type;
		RW lo, hi;
		bool done;
		Atoms(int l, int h, int n)
			: lo(l, n), hi(h, n), done(false)
		{ 
			assert(n >= 0);
			assert(l <= h);
		}
		bool operator==(const Atoms& a) const
		{
			return lo == a.lo and hi == a.hi;
		}
		bool operator!=(const Atoms& a) const
		{
			return !operator==(a);
		}

		explicit operator bool() const
		{
			return !done;
		}
		value_type operator*() const
		{
			return lo;
		}
		Atoms& operator++()
		{
			if (lo == hi) {
				done = true;
			}
			else {
				++lo;
			}

			return *this;
		}
	};
	Atoms operator()(const RW& o) const
	{
		int t = time(o);
		assert(t <= T);

		return Atoms(o(t) - T + t, o(t) + T - t, T);
	}
};

struct RandomWalk {
	typedef RW atom_type;
	double operator()(const atom_type& a) const
	{
		return a.prob();
	}
};

template<class Model>
struct Stock {
	double s, v;
	Stock(double s, double v)
		: s(s), v(v)
	{ }
	auto operator()(int t) const
	{
		return [*this,t](const typename Model::atom_type& o) {
			return s * exp(v*o(t))/pow(cosh(v), t);
		};
	}
};

template<class Model>
struct Put {
	const Stock<Model>& S;
	double k;
	int t;
	Put(const Stock<Model>& S, double k, int t)
		: S(S), k(k), t(t)
	{ }
	auto operator()(int u) const
	{
		assert(u == t);
		return[*this](const typename Model::atom_type& o) {
			return std::max(k - S(t)(o), 0.);
		};
	}
};

int main()
{
	constexpr double eps = std::numeric_limits<double>::epsilon();
	ConstantStoppingTime T(3);
	Stock<RandomWalk> S(100, 0.1);
	RandomWalk W;

	{
		auto E = expected_value(T, S, W);
		double E0 = E(RW(0, 0));
		assert(fabs(E0 - 100) <= 200 * eps);
		//E0 = E(RW(-3, 3));
		//assert(E(RW(-3, 3)) == S(3)(RW(-3, 3)));
	}
	{
		int t = 3;
		Put P(S, 100, t);
		auto E = expected_value(ConstantStoppingTime(t), P, W);
		double E0 = E(RW(0, 0));
		assert(fabs(E0 - 7.450) < .001);
	}

	return 0;
}
