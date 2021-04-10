// distribution.h - random variable distributions
#pragma once
#include <concepts>
#include <type_traits>

namespace polyfin {

	template<class D>
	concept distribution = requires(D d) {
		typename D::xtype;
		typename D::stype;
		// cumulative distribution function and derivatives
		{ d.cdf(D::xtype,D::stype,std::size_t) const } -> std::same_as<D::xtype>;
		// Esscher distribution function d/ds P_s(X <= x)
		{ d.edf(D::xtype,D::stype) const } -> std::same_as<D::xtype>;
		// cumulant generating function and derivatives
		{ d.cgf(D::stype,std::size_t) const } -> std::same_as<D::stype>;
	};

} // namespace polyfin
