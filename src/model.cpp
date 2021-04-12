// model.cpp - unified model
#include <cassert>
#include <cmath>
//#include <concepts>
#include <iostream>
#include <iterator>
#include <tuple>

/*
template<class Atom>
concept Filtered = requires (Atom a) {
	typename Atom::time_type;
	typename Atom::prob_type;
	{ a.time() -> time_type; };
	{ a.prob() -> prob_type; };
};
*/

//  time associated with an atom
template<class Atom, class Time = typename Atom::time_type>
Time time(const Atom& a)
{
	return a.time();
}

// probability associated with an atom
template<class Atom, class Prob = typename Atom::prob_type>
Prob prob(const Atom& a)
{
	return a.prob();
}

// a stopping time T:Omega -> time is a collection of atoms of Omega
// with T(omega) = time(omega)
// atom A: B\subset A and time(B) = time(A) implies B = A or B empty

// E[X_T](omega), E[1_T](omega)
template<class StoppingTime, class StochasticProcess, class Measure>
auto expected_value2(const StoppingTime& T, const StochasticProcess& X, const Measure& P)
{
	return [&T, &X, &P](const Measure::atom_type& omega) -> std::pair<double,double> {
		double EX = 0, E1 = 0;

		for (/* Atom */auto o = T(omega); o; ++o) {
			double po = prob(*o);
			E1 += po;
			EX += X(time(*o))(*o) * po;
		}

		return { EX, E1 };
	};
}
// E[X_T](omega)
template<class StoppingTime, class StochasticProcess, class Measure>
auto expected_value(const StoppingTime& T, const StochasticProcess& X, const Measure& P)
{
	const auto& E2 = expected_value2(T, X, P);

	return [&E2](const auto& omega) {
		auto [EX, E1] = E2(omega);

		return EX/E1;
	};
}

// measure on (w_j) in {-1,1}^infty
struct RandomWalk {
	typedef int time_type;
	// w_1 + ... w_n = k
	class Atom {
		typedef typename RandomWalk::time_type time_type;
		typedef double prob_type;
		time_type k, n;
	public:
		Atom(time_type k = 0, time_type n = 0)
			: k(k), n(n)
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

		//auto operator<=>(const Atom&) const = default;
		bool operator==(const Atom& o) const
		{
			return k == o.k and n == o.n;
		}
		bool operator!=(const Atom& o) const
		{
			return !operator==(o);
		}

		time_type time() const
		{
			return n;
		}
		// level_type ???
		time_type operator()(time_type t) const
		{
			assert(t == n);

			return k;
		}

		Atom& operator++()
		{
			if (k < n) {
				k += 2;
			}

			return *this;
		}

		// C(n, k_)/2^n
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
	typedef Atom atom_type;
	// atoms from lo to hi at time n
	struct Atoms {
		typedef std::forward_iterator_tag iterator_category;
		typedef RandomWalk::Atom value_type;
		typedef typename RandomWalk::time_type time_type; 
		value_type lo, hi;
		bool done;
		Atoms(time_type l, time_type h, time_type n)
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
	// atoms at time T contained in o
	RandomWalk::Atoms atoms(time_type T, const RandomWalk::Atom& o) const
	{
		time_type t = time(o);
		assert(t <= T);

		return RandomWalk::Atoms(o(t) - T + t, o(t) + T - t, T);
	}
};


template<class Measure>
class ConstantStoppingTime {
	typedef typename Measure::Atom::time_type time_type;
	typedef typename Measure::atom_type atom_type;
	const Measure& m;
	time_type T;
public:
	ConstantStoppingTime(const Measure& m, time_type T = 0)
		: m(m), T(T)
	{ }
	ConstantStoppingTime(const ConstantStoppingTime&) = default;
	ConstantStoppingTime& operator=(const ConstantStoppingTime&) = default;
	~ConstantStoppingTime()
	{ }
	// atoms at time T contained in o
	auto operator()(const atom_type& o) const
	{
		return m.atoms(T, o);
	}
};


template<class Model>
struct Futures {
	typedef typename Model::atom_type atom_type;
	typedef typename Model::Atom::time_type time_type;

	double s, v;
	Futures(double s, double v)
		: s(s), v(v)
	{ }
	Futures(const Futures&) = default;
	Futures& operator=(const Futures&) = default;
	~Futures()
	{ }
	auto operator()(time_type t) const
	{
		return [*this,t](const atom_type& o) {
			return s * exp(v*o(t))/pow(cosh(v), t);
		};
	}
};

template<class Model>
struct Put {
	typedef typename Model::Atom::time_type time_type;

	const Futures<Model>& S;
	double k;
	time_type t;
	Put(const Futures<Model>& S, double k, time_type t)
		: S(S), k(k), t(t)
	{ }
	Put(const Put&) = default;
	Put& operator=(const Put&) = default;
	~Put()
	{ }
	auto operator()(time_type u) const
	{
		assert(u == t);
		return[*this](const typename Model::Atom::atom_type& o) {
			return std::max(k - S(t)(o), 0.);
		};
	}
};

int main()
{
	constexpr double eps = std::numeric_limits<double>::epsilon();
	RandomWalk W;
	ConstantStoppingTime T(W, 3);
	Futures<RandomWalk> S(100, 0.1);

	{
		auto E = expected_value(T, S, W);
		double E0 = E(RandomWalk::Atom(0, 0));
		assert(fabs(E0 - 100) <= 200 * eps);
		//E0 = E(RandomWalk::Atom(-3, 3));
		//assert(E(RandomWalk::Atom(-3, 3)) == S(3)(RandomWalk::Atom(-3, 3)));
	}
	{
		int t = 3;
		Put P(S, 100, t);
		auto E = expected_value(T, P, W);
		double E0 = E(RandomWalk::Atom(0, 0));
		assert(fabs(E0 - 7.450) < .001);
	}

	return 0;
}
