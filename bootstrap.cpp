// bootstrap.cpp - bootstrap a curve
#include <cassert>
#include <chrono>
#include <compare>
#include <iterator>
#include <limits>
#include <iostream>

namespace fms {

template<class I>
concept iterable =  // forward_iterable
	requires (I i) {
	//std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
	//typename I::iterator_concept;
	typename I::iterator_category;
	typename I::value_type;
	{ !!i } -> std::same_as<bool>;
	{  *i } -> std::convertible_to<typename I::value_type>; // remove_cv???
	{ ++i } -> std::same_as<I&>;
	{ i++ } -> std::same_as<I>;
	//{ i.operator==(i) const } -> std::same_as<bool>;
	//{ i.end() } -> std::same_as<I>;
};

//!!! concept input_iterable, etc

// t, t + dt, ...
template<class T, class dT = T>
class sequence {
	T t;
	dT dt;
public:
	using iterator_category = std::input_iterator_tag; // ??? bidirectional
	using value_type = T;
	using difference_type = dT;

	// default 0, 1, ...
	sequence(const T& t = T(0), const dT& dt = dT(1))
		: t(t), dt(dt)
	{ }
	sequence(const sequence&) = default;
	sequence& operator=(const sequence&) = default;
	~sequence()
	{ }

	bool operator<=>(const sequence&) const = default;

	sequence begin() const
	{
		return *this;
	}
	sequence end() const
	{
		return sequence(t, -dt); // ??? std::numeric_limits<T>::max()
	}

	explicit operator bool()
	{
		return true;
	}
	value_type operator*() const
	{
		return t;
	}
	sequence& operator++()
	{
		t += dt;

		return *this;
	}
	sequence operator++(int)
	{
		sequence s_{*this};
		operator++();

		return s_;
	}

	// operator--
	// operator+=
	// operator-=
	// operator+
	// operator-
#ifdef _DEBUG
	static int test()
	{
		{
			sequence<T> s;
			assert(s);
			sequence s2{s};
			assert(s2);
			s = s2;
			assert(s);
			assert(*s == *s2);
			assert(0 == *s++);
			assert(1 == *s++);
			assert(2 == *s++);
		}
		{
			sequence<T> s(1,2);
			assert(s);
			sequence s2{s};
			assert(s2);
			s = s2;
			assert(s);
			assert(*s == *s2);
			assert(1 == *s++);
			assert(3 == *s++);
			assert(5 == *s++);
		}
		/*
		{
//using namespace std::chrono;
			sequence s(date{2001/January/1}, month{1});
		}
		*/

		return 0;
	}
#endif // _DEBUG
};
int test_sequence_i = sequence<int>::test();
int test_sequence_d = sequence<double>::test();
int test_sequence_f = sequence<float>::test();

// raw pointer
template<class T>
class ptr {
	T* t;
public:
	using iterator_category = std::input_iterator_tag;
	using value_type = T;

	ptr(T* t = nullptr)
		: t(t)
	{ }
	ptr(const ptr&) = default;
	ptr& operator=(const ptr&) = default;
	~ptr()
	{ }

	auto operator<=>(const ptr&) const = default;

	ptr begin() const
	{
		return *this;
	}
	ptr end() const
	{
		return ptr{};
	}

	explicit operator bool() const
	{
		return t != nullptr;
	}
	value_type operator*() const
	{
		return *t;
	}
	ptr& operator++()
	{
		if (operator bool()) {
			++t;
		}

		return *this;
	}
	ptr operator++(int)
	{
		ptr t_{*this};
		operator++();

		return t_;
	}

#ifdef _DEBUG
static int test()
{
	{
		ptr<int> a;
		assert(!a);
		ptr<int> a2{a};
		assert(!a2);
		a = a2;
		++a;
		assert(!a);
		a++;
		assert(!a);
	}
	{
		int i[] = {1,2,3};
		ptr a(i);
		assert(a);
		ptr a2{a};
		assert(a2);
		a = a2;
		assert(1 == *a);
		++a;
		assert(2 == *a++);
		assert(3 == *a);
		assert(++a); // no bounds checking
		// assert(*a); // undefined behavior
	}

	return 0;
}
#endif // _DEBUG
};
int test_ptr = ptr<int>::test();

template<iterable I>
class take {
	size_t n;
	I i;
public:
	using iterator_category = typename I::iterator_category;
	using value_type = typename I::value_type;

	take(size_t n, const I& i)
		: n(n), i(i)
	{ }
	take(const take&) = default;
	take& operator=(const take&) = default;
	~take()
	{ }

	auto operator<=>(const take&) const = default;

	take begin() const
	{
		return *this;
	}
	take end() const
	{
		return take(0, i);
	}

	explicit operator bool() const
	{
		return n != 0 and i;
	}
	value_type operator*() const
	{
		return *i;
	}
	take& operator++()
	{
		if (operator bool()) {
			--n;
			++i;
		}

		return *this;
	}
	take operator++(int)
	{
		take t_{*this};
		operator++();

		return t_;
	}
};

template<iterable I>
inline I drop(size_t n, I i)
{
	while (n--)
		++i;

	return i;
}

template<class T>
inline auto array(size_t n = 0, T* t = nullptr)
{
	return take(n, ptr(t));
}
template<size_t N, class T>
inline auto array(T(&t)[N])
{
	return take(N, ptr(t));
}

#ifdef _DEBUG
template<class T>
int test_array()
{
	{
		auto a = array<T>();
		assert(!a);
		auto a2{a};
		assert(!a2);
		a = a2;
		assert(equal(a,a2));
		++a;
		assert(!a);
		a++;
		assert(!a);
	}
	{
		T i[] = {1,2,3};
		auto a = array(3, i);
		assert(a);
		assert(3 == length(a));
		auto a2 = a;
		assert(a2);
		assert(3 == length(a2));
		a = a2;
		assert(1 == *a);
		++a;
		assert(2 == *a++);
		assert(3 == *a);
		assert(!++a);

	}
	{
		T i[] = {1,2,3};
		auto a = array(3, i);
		auto b = array(i);
		assert(equal(a,b));
		assert(equal(a,array(i)));
	}

	return 0;
}
int test_array_i = test_array<int>();
#endif // _DEBUG

template<iterable I>
inline size_t length(I i, size_t n = 0)
{
	while (i++)
		++n;

	return n;
}
#ifdef _DEBUG
template<iterable I, iterable J>
inline int test_length(I i, J j)
{
	if (!i) {
		assert(0 == length(i));
	}
	assert(length(i, length(j)) == length(i) + length(j));

	return 0;
}
#endif // _DEBUG

template<iterable I, iterable J>
bool equal(I i, J j)
{
	while (i and j) {
		if (*i++ != *j++)
			return false;
	}

	return !i and !j;
}

template<iterable I, iterable J>
class pair {
	I i;
	J j;
public:
	using iterator_category = std::input_iterator_tag; // common_type ???
	using value_type = std::pair<typename I::value_type, typename J::value_type>;
	pair(const I& i, const J& j)
		: i(i), j(j)
	{ }
	pair(const pair&) = default;
	pair& operator=(const pair&) = default;
	~pair()
	{ }

	auto operator<=>(const pair&) const = default;

	pair begin() const
	{
		return pair(i.begin(), j.begin());
	}
	pair end() const
	{
		return pair(i.end(), j.end());
	}

	explicit operator bool() const
	{
		return i and j;
	}
	value_type operator*() const
	{
		return std::pair(*i, *j);
	}
	pair& operator++()
	{
		++i;
		++j;

		return *this;
	}
	pair operator++(int)
	{
		pair p_{*this};
		operator++();

		return p_;
	}
};

// f(t) = x[i], t[i-1] < t <= t[i]
template<iterable T, iterable X, class _T = typename T::value_type, class _X = typename X::value_type>
class pwflat {
	T t;
	X x;
	_X _x; // extrapolate
	//static inline constexpr _X NaN = std::numeric_limits<_X>::quiet_NaN();
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
		return std::pair(*t, *x);
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
		pwflat f_{*this};
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
			if (t_ <= t0) {
				operator++();
			}
			else if (t_ <= _t) {
				I += x*(t_ - t0);
				t0 = t_;
				operator++();
			}
			else {
				I += x*(_t - t0);

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

	// t r(t) = int_0^t f(s) ds
	_X spot(const _T& _t) const
	{
		const auto& [t_,x_] = operator*();

		return _t <= t_ ? x_ : integral(_t)/_t;
	}

	// pv and discount to last payment, increment this and i
	std::pair<_X,_X> present_value(pair<T,X>& i, const _T& _u = std::numeric_limits<T>::infinity())
	{
		_T t0 = 0;
		_X pv = 0;
		_X D = 1;

		while (i) {
			const auto& [u,c] = *i;
			if (!operator bool() or u > _u) {
				break;
			}
			D *= exp(-integrate(u, t0));
			pv += c * D;
			t0 = u;
			++i;
		}

		return std::pair(pv, D);
	}

};
int test_pwflat()
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
int test_pwflat_ = test_pwflat();

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

int main()
{
	return 0;
}

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
