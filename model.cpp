// model.cpp - unified model
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <tuple>


// concept Atom requires (A a) { a.time(); a.P() }

//  time associated with an atom
template<class Atom, class Time = double> // Atom::time_type
Time time(const Atom& a)
{
	return a.time();
}

// probability associated with an atom
template<class Atom, class Time = double> // Atom::time_type
double P(const Atom& a)
{
	return a.P();
}

// E[X_T](omega)
template<class StoppingTime, class StochasticProcess, class Measure>
auto conditional_expectation(const StoppingTime& T, const StochasticProcess& X, const Measure& P)
{
	return [&T, &X, &P](const auto& omega) {
		double E = 0;

		for (auto o = T(omega); o; ++o) {
			E += X(time(*o))(*o) * P(*o);	
		}

		return E;
	};
}

// random walk atoms
class RW {
public:
	int k, n;
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
		return t == n ? k : std::numeric_limits<int>::max();
	}

	RW& operator++()
	{
		if (k < n) {
			k += 2;
		}

		return *this;
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
		Atoms(int l, int h, int n)
			: lo(l, n), hi(h, n)
		{ }
		bool operator==(const Atoms& a) const
		{
			return lo == a.lo and hi == a.hi;
		}
		bool operator!=(const Atoms& a_) const
		{
			return !operator==(a_);
		}
		explicit operator bool() const
		{
			return lo(time(lo)) <= hi(time(hi));
		}
		value_type operator*() const
		{
			return lo;
		}
		Atoms& operator++()
		{
			++lo;

			return *this;
		}
	};
	Atoms operator()(const RW& o) const
	{
		int t = time(o);
		assert(t <= T);

		return Atoms(o(T) - T + t, o(T) + T - t, T);
	}
};


struct RandomWalk {
	double operator()(const RW& a)
	{
		return a.P();
	}
};


int main()
{
	return 0;
}
