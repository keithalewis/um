// bootstrap.cpp - bootstrap a curve
#include <limits>
#include "iterable.h"

namespace fms {
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