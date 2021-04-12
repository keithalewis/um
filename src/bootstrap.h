// bootstrap.cpp - bootstrap a curve
#include <limits>
#include "iterable.h"

namespace fms {

	// f(t) = x[i], t[i-1] < t <= t[i]
	template<iterable T, iterable X, 
		class _T = typename T::value_type, class _X = typename X::value_type>
	class pwflat {
		T t;
		X x;
		_X _x; // extrapolate
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<_T, _X>;

		pwflat(const T& t, const X& x, const _X& _x = std::numeric_limits<_X>::quiet_NaN())
			: t(t), x(x), _x(_x)
		{ }
		pwflat(const pwflat&) = default;
		pwflat& operator=(const pwflat&) = default;
		~pwflat()
		{ }

		auto operator<=>(const pwflat& f) const = default;

		pwflat begin() const
		{
			return pwflat(t.begin(), x.begin());
		}
		pwflat end() const
		{
			return pwflat(t.end(), x.end());
		}

		explicit operator bool() const
		{
			return t and x;
		}
		value_type operator*() const
		{
			return value_type(*t, *x);
		}
		pwflat& operator++()
		{
			if (operator bool()) {
				++t;
				++x;
			}

			return *this;
		}
		pwflat operator++(int)
		{
			pwflat f_{ *this };
			operator++();

			return f_;
		}

		// value and advance to _t
		value_type operator()(const _T& _t)
		{
			while (operator bool()) {
				const auto& [t_,x_] = operator*();
				if (_t <= t_) {
					return value_type(t_,x_);
				}
				operator++();
			}

			return value_type(_t, _x); // extrapolate
		}
		// value and don't modify curve
		value_type operator()(const _T& _t) const
		{
			pwflat f{*this};

			return f.value(_t);
		}

		// integrate and advance to _t
		_X integrate(const _T& _t, const _T& t0 = _T(0))
		{
			_X I = 0;

			while (operator bool()) {
				const auto& [t_,x_] = operator*();
				if (t_ < t0) {
					operator++();
				}
				else if (t_ <= _t) {
					I += x_*(t_ - t0);
					t0 = t_;
					operator++();
				}
				else {
					I += x_*(_t - t0);

					break;
				}
			}

			return I;
		}

		// integrate but don't advance
		_X integral(const _T& _t, const _T& t0 = _T(0)) const
		{
			pwflat f{*this};

			return f.integrate(_t, t0);
		}

		_X discount(const _T& _t, const _T& t0 = _T(0)) const
		{
			return exp(-integral(_t, t0));
		}

		// t r(t) = int_0^t f(s) ds
		_X spot(const _T& _t) const
		{
			const auto& [t_,x_] = operator*();

			return _t <= t_ ? x_ : integral(_t)/_t;
		}

		// pv and discount to last cash flow
		std::pair<_X,_X> present_value(T& u, X& c, const _T& t0 = _T(0))
		{
			_X pv = 0;
			_X D = 1;

			while (u and c) {
				if (*u >= t0) {
					D *= exp(-integrate(*u, t0));
					pv += *c * D;
					t0 = *u;
				}
				++u;
				++c;
			}

			return std::pair(pv, D);
		}

	};
#ifdef _DEBUG
	static int test()
	{
		{
			double t[] = {1,2,3};
			double x[] = {.1,.2,.3};
			pwflat f(array(t), array(x));
			assert(f);
			pwflat f2{f};
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
	/*
	// p = pvi + Di * sum c * exp(-f(u - _u))
	template<class F, class I, class T, class X>
	inline std::pair<T,X> bootstrap(F& f, const I& i, const X& p)
	{
		const auto& [_pv, _D] = present_value(f, i);
		const auto& [_u, _x] = *f;
		auto pv = [f,i,p,pv0](X x_) {
			X pv_ = present_value(pwflat(x_), I(i), _u);

			return _D*pv_ - _pv - p;
		}

		return [?,root1d(pv)];
	}
	*/

} // namespace fms
	/*
		struct fra {
			T effective;
			P period;
			DCF dcf;
			auto operator()(const Deflator& D, T t) const
			{
				return (D(t, effective)/D(t, effective + period) - 1)/dcf(effective, effective + period);
			}
		};
	*/