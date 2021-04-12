// iterable.h - iterator with explicit operator bool() const
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <compare>
#include <concepts>
#include <iterator>
#include <type_traits>

namespace fms {

	//!!! concept input_iterable, etc
	template<class I>
	concept iterable =  // input_iterable
		std::is_base_of_v<std::input_iterator_tag, typename I::iterator_category> &&
		requires (I i) {
		//typename I::iterator_concept;
		typename I::value_type;
		{ !!i } -> std::same_as<bool>;
		{  *i } -> std::convertible_to<typename I::value_type>; // remove_cv???
		{ ++i } -> std::same_as<I&>;
		{ i++ } -> std::same_as<I>;
		//{ i.operator==(i) const } -> std::same_as<bool>;
		//{ i.end() } -> std::same_as<I>;
	};

	template<iterable I, iterable J>
	inline constexpr bool equal(I i, J j)
	{
		while (i and j) {
			if (*i++ != *j++)
				return false;
		}

		return !i and !j;
	}

	// increment until first item satisfying p
	template<iterable I, class P>
	inline constexpr I until(I i, const P& p)
	{
		return !i or p(i) ? i : until(++i, p);
	}

	// return end or first false item
	template<iterable I>
	inline constexpr I all(I i)
	{
		return until(i, [](I i) { return !*i; });
	}
	// return end or first true item
	template<iterable I>
	inline constexpr I any(I i)
	{
		return until(i, [](I i) { return *i; });
	}

	// filter on predicate p
	template<iterable I, class P>
	class when {
		I i;
		const P& p;
	public:
		using iterator_category = typename I::iterator_category;
		using value_type = typename I::value_type;

		when(const I& i_, const P&p)
			: i(i_), p(p)
		{
			i = until(i, p);
		}

		bool operator==(const when& w) const
		{
			return i == w.i and &p == &w.p;
		}

		when begin() const
		{
			return when(i, p);
		}
		when end() const
		{
			return when(i.end(), p);
		}

		explicit operator bool() const
		{
			return !!i;
		}
		value_type operator*() const
		{
			return *i;
		}
		when& operator++()
		{
			i = until(++i, p);

			return *this;
		}
		when operator++(int)
		{
			when w_(i, p);
			operator++();

			return w_;
		}
	};

#ifdef _DEBUG

	template<iterable I>
	inline bool test_all(const I& is)
	{
		bool b = true; // all({}) = true

		for (auto i : is) {
			if (!i) {
				b = false;
				break;
			}
		}

		auto as = all(is);

		if (b) {
			assert(!as);
			assert(as == is.end());
		}
		else {
			assert(as);
			assert(!*as);
		}
	}

#endif // _DEBUG

	template<iterable I>
	inline constexpr size_t length(I i, size_t n = 0)
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
		assert(length(j, length(i)) == length(i) + length(j));

		return 0;
	}
	#endif // _DEBUG

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

		auto operator<=>(const sequence&) const = default;

		sequence begin() const
		{
			return *this;
		}
		sequence end() const
		{
			return sequence(t, -dt); // ??? std::numeric_limits<T>::max()
		}

		explicit operator bool() const
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

	template<class T>
	inline sequence<T> constant(const T& t)
	{
		return sequence(t,0);
	}
	template<class T>
	inline sequence<T> c(const T& t)
	{
		return sequence(t,0);
	}

	// raw pointer
	template<class T>
	class ptr {
		T* t;
	public:
		using iterator_category = std::input_iterator_tag; //!!! bidirectional
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
			return (n != 0) and i;
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
	#ifdef _DEBUG
		static int test()
		{
			{
				typename I::value_type p[] = {1,2,3};
				auto t = take(3, ptr(p));
				assert(t);

				auto t2{t};
				assert(t2);
				assert(t == t2);
				assert(!(t != t2));

				t = t2;
				assert(t);
				assert(t == t2);
				assert(!(t != t2));

				assert(1 == *t);
				++t;
				assert(t);
				assert(2 == *t);

				assert(2 == *t++);
				assert(3 == *t);

				assert(!++t);
			}

			return 0;
		}
	#endif // _DEBUG
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
	#endif // _DEBUG

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

	template<class F, iterable I>
	class apply {
		const F& f;
		I i;
	public:
		using iterator_category = typename I::iterator_category;
		using value_type = std::invoke_result_t<F,typename I::value_type>;
		apply(const F& f, const I& i)
			: f(f), i(i)
		{ }

		explicit operator bool() const
		{
			return !!i;
		}
		auto operator*() const
		{
			return f(*i);
		}
		apply& operator++()
		{
			++i;

			return *this;
		}
		apply& operator++(int)
		{
			apply a_(f, i);
			operator++();

			return a_;
		}
	#ifdef _DEBUG
		static int test()
		{
			{
				apply a([](auto x) { return x*x; }, sequence<int>());
				assert(a);
				assert(0 == *a);
				assert(1 == *++a);
				assert(4 == *++a);
			}

			return 0;
		}
	#endif // _DEBUG
	};

} // namespace polyfin
