// fms_distribution.h - random variable distributions
#pragma once
#include <concepts>
#include <type_traits>

namespace fms {

	template<class D>
	concept distribution = requires(D d) {
		typename D::xtype;
		typename D::stype;
		// cumulative distribution function and derivatives
		{ d.cdf(D::xtype, D::stype, size_t) } -> std::same_as<typename D::xtype>;
		// Esscher derivative function d/ds P_s(X <= x)
		{ d.edf(D::xtype, D::stype) } -> std::same_as<typename D::xtype>;
		// cumulant generating function and derivatives
		{ d.cgf(D::stype, size_t) } -> std::same_as<typename D::stype>;
	};

} // namespace fms
