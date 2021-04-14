// fms_pwflat.cpp - piecewise flat curve
#include <math.h>
#include <algorithm>
#include <limits>
#include "fms_iterable.h"

using namespace fms::iterable;

namespace fms::pwflat {

	template<class T>
	inline constexpr T NaN() { return std::numeric_limits<T>::quiet_NaN(); }

	// first pair [t,x] with t >= _t and advance t, x
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	inline std::pair<_T,_X> valuate(T& t, X& x, const _T& _t, const _X& _x = NaN<_X>())
	{
		while (t and x) {
			if (*t >= _t) {
				return std::pair(*t, *x);
			}
			++t;
			++x;
		}

		return std::pair(NaN<_T>(), _x);
	}
	
	// first pair [t,x] with t >= _t
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	inline std::pair<_T, _X> value(const T& t, const X& x, const _T& _t, const _X& _x = NaN<_X>())
	{
		T t_{ t };
		X x_{ x };

		return valuate(t_, x_, _t, _x);
	}
	
#ifdef _DEBUG
	inline int test_value()
	{
		{

			double t[] = { 1., 2., 3. };
			double x[] = { .1, .2, .3 };

			double _t = -1;
			auto tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[0], x[0]));

			_t = 0;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[0], x[0]));

			_t = 0.5;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[0], x[0]));

			_t = 1;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[0], x[0]));

			_t = 1.5;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[1], x[1]));

			_t = 2;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[1], x[1]));

			_t = 2.1;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[2], x[2]));

			_t = 3;
			tx = value(array(t), array(x), _t);
			assert(tx == std::pair(t[2], x[2]));

			_t = 3.1;
			tx = value(array(t), array(x), _t);
			assert(isnan(tx.first));
			assert(isnan(tx.second));
		}
		{
			auto t = take(3, sequence(1., 1.));
			auto x = take(3, sequence(.1, .1));

			auto gt3 = [](auto i) { return i > 3; };
			for (const double& _t : until(sequence(-1., .1), gt3)) {
				auto t_{ t };
				auto x_{ x };

				assert(value(t, x, _t) == valuate(t_, x_, _t));
			}

		}
		{
			auto f = [](double t) { return ceil(t) * .1; };
			auto t = sequence(0., 1.);
			auto x = sequence(0., .1);
			auto ts = until(sequence(-1., .1), [](auto i) { return i > 3; });
			for (const double& _t : ts) {
				const auto& [t_, x_] = valuate(t, x, _t);
				assert(f(t_) == x_);
			}
		}

		return 0;
	}
#endif // _DEBUG

	// integrate x(t) from t0 to _t and advance t, x
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	inline _X integrate(T& t, X& x, const _T& _t, const _T& t0 = _T(0))
	{
		_X I = 0;

		while (t and x) {
			if (*t > _t) {
				I += *x * (_t - t0);

				break;
			}
			else if (t0 < *t and *t <= _t) {
				I += *x * (*t - t0);
				t0 = *t;
			}
			++t;
			++x;
		}

		return I;
	}
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	inline _X integral(const T& t, const X& x, const _T& _t, const _T& t0 = _T(0))
	{
		T t_{ t };
		X x_{ x };

		return integrate(t_, x_, _t, t0);
	}

	// pv and discount to last cash flow
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	std::pair<_X, _X> present_valuate(T& t, X& x, T& u, X& c, const _T& t0 = _T(0))
	{
		_X pv = 0;
		_X D = 1;

		while (t and x and u and c) {
			if (*u >= t0) {
				D *= exp(-integrate(t, x, *u, t0));
				pv += *c * D;
				t0 = *u;
			}
			++u;
			++c;
		}

		return std::pair(pv, D);
	}
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	std::pair<_X, _X> present_value(const T& t, const X& x, const T& u, const X& c, const _T& t0 = _T(0))
	{
		T t_{ t }, u_{ u };
		X x_{ x }, c_{ c };

		return present_valuate(t_, x_, u_, c_, t0);
	}

	// f(t) = x[i], t[i-1] < t <= t[i]
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	struct curve {
		T t;
		X x;
		_X _x; // extrapolate
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<_T, _X>;

		curve(const X& x)
			: t(0), x(0), _x(x)
		{ }
		curve(const T& t, const X& x, const _X& _x = std::numeric_limits<_X>::quiet_NaN())
			: t(t), x(x), _x(_x)
		{ }
		curve(const curve&) = default;
		curve& operator=(const curve&) = default;
		~curve()
		{ }

		bool operator==(const curve& f) const
		{
			return ((isnan(_x) and isnan(f._x)) or _x == f._x) and t == f.t and x == f.x;
		}

		curve begin() const
		{
			return curve(t.begin(), x.begin());
		}
		curve end() const
		{
			return curve(t.end(), x.end());
		}

		explicit operator bool() const
		{
			return t and x;
		}
		value_type operator*() const
		{
			return value_type(*t, *x);
		}
		curve& operator++()
		{
			if (operator bool()) {
				++t;
				++x;
			}

			return *this;
		}
		curve operator++(int)
		{
			curve f_{ *this };
			operator++();

			return f_;
		}

		// value and advance to _t
		value_type operator()(const _T& _t)
		{
			return valuate(t, x, _t, _x);
		}
		// value and don't modify curve
		value_type operator()(const _T& _t) const
		{
			return value(t, x, _t, _x);
		}

		// integrate and advance to _t
		_X integrate(const _T& _t, const _T& t0 = _T(0))
		{
			return integrate(t, x, t0);
		}

		// integrate but don't advance
		_X integral(const _T& _t, const _T& t0 = _T(0)) const
		{
			return integral(t, x, t0);
		}

		_X discount(const _T& _t, const _T& t0 = _T(0)) const
		{
			return exp(-integral(_t, t0));
		}

		// t r(t) = int_0^t f(s) ds
		_X spot(const _T& _t) const
		{
			const auto& [t_, x_] = operator*();

			return _t <= t_ ? x_ : integral(_t) / _t;
		}

		// pv and discount to last cash flow
		std::pair<_X, _X> present_valuate(T& u, X& c, const _T& t0 = _T(0))
		{
			return present_valuate(t, x, u, c, t0);
		}
		std::pair<_X, _X> present_value(T& u, X& c, const _T& t0 = _T(0)) const
		{
			return present_value(t, x, u, c, t0);
		}

	};
#ifdef _DEBUG
	static int test()
	{
		{
			double t[] = { 1,2,3 };
			double x[] = { .1,.2,.3 };
			curve f(array(t), array(x));
			assert(f);
			curve f2{ f };
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
#endif // _DEBUG

} // namespace fms
