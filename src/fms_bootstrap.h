// fms_bootstrap.cpp - bootstrap a curve
#include <limits>
#include "fms_iterable.h"
#include "fms_pwflat.h"

using namespace fms::iterable;

namespace fms::bootstrap {

	inline const char* next_doc = R"(
Given a piecewise flat curve t, x find next point that reprices u, c.
\(p = pv_0 + D_0 \sum c * exp(-f(u - u0))\).
)";
	template<input_iterable T, input_iterable X,
		class _T = typename T::value_type, class _X = typename X::value_type>
	inline std::pair<_T, _X> next(T& t, X& x, T& u, X& c, const _X& _x, const _X& p = _X(0))
	{
		const auto& [pv0, D0] = present_valuate(t, x, u, c);
		auto F = [p, pv0, D0, u, c](_X f) { 
			pwflat::curve F(f);  
			return -p + pv0 + D0*f.present_value(u, c); 
		};

		return std::pair(0, _x);
	}


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