// fms_iterable.h - iterator with explicit operator bool() const
#pragma once
#ifdef _DEBUG
#include <cassert>
#include <chrono>
#endif
#include <compare>
#include <concepts>
#include <functional>
#include <iterator>
#include <type_traits>

namespace fms::iterable {

	template<class T>
	concept pointer_iterable = std::input_iterator<T*>;

	//!!! concept input_iterable, etc 
	// pointer_iterable has unsafe operator bool() always returning true
	template<class I>
	concept input_iterable =  // input_iterable
		std::is_base_of_v<std::input_iterator_tag, typename I::iterator_category> &&
		// ??? std::derived_from<std::iterator_traits<I>, std::input_iterator_tag> &&
		requires (I i) {
			typename I::iterator_category;
			typename I::difference_type;
			typename I::value_type;
			typename I::pointer;
			typename I::reference;
			{ !!i } -> std::same_as<bool>; // operator bool()
			{  *i } -> std::convertible_to<typename I::value_type>; // remove_cv???
			{ ++i } -> std::same_as<I&>;
			{ i++ } -> std::same_as<I>;
			{ i.operator==(i) } -> std::same_as<bool>;
			{ i.begin() } -> std::same_as<I>;
			{ i.end() } -> std::same_as<I>;
		};

	// All iterables begin alike...
	template<input_iterable I>
	inline I begin(I i)
	{
		return i;
	}
	// ...but each ends after its own fashion.
	template<input_iterable I>
	inline I end(I i)
	{
		return i.end();
	}

	// iterables are lightweight and often passed by value
	
	// return end or last item before end
	template<input_iterable I>
	inline constexpr I back(I i)
	{
		while (I _i = i++)
			if (!i)
				return _i;

		return i;
	}

	// all iterable items are equal
	template<input_iterable I, input_iterable J>
	inline constexpr bool equal(I i, J j)
	{
		while (i and j)
			if (*i++ != *j++)
				return false;

		return !i and !j;
	}

	// return end or first item satisfying p
	template<input_iterable I, class P>
	inline constexpr I upto(I i, const P& p)
	{
		return !i or p(i) ? i : upto(++i, p);
	}

	// return end or first false item
	template<input_iterable I>
	inline constexpr I all(I i)
	{
		return upto(i, [](I i) { return !*i; });
	}
	// return end or first true item
	template<input_iterable I>
	inline constexpr I any(I i)
	{
		return upto(i, [](I i) { return *i; });
	}

	// filter on predicate p
	template<input_iterable I, class P>
	class when {
		I i;
		const P& p;
	public:
		using iterator_category = std::input_iterator_tag;
		using difference_type = typename I::difference_type;
		using value_type = typename I::value_type;
		using pointer = typename I::pointer;
		using reference = typename I::reference;

		when(const I& i, const P&p)
			: i(upto(i, p)), p(p)
		{ }

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
			if (i) {
				i = upto(++i, p);
			}

			return *this;
		}
		when operator++(int)
		{
			when w_(i, p);
			operator++();

			return w_;
		}
	};

	// stop when p is true
	template<input_iterable I, class P>
	class until {
		I i;
		const P& p;
	public:
		using iterator_category = std::input_iterator_tag;
		using difference_type = typename I::difference_type;
		using value_type = typename I::value_type;
		using pointer = typename I::pointer;
		using reference = typename I::reference;

		until(const I& i, const P& p)
			: i(i), p(p)
		{ }

		bool operator==(const until& u) const
		{
			return i == u.i and &p == &u.p;
		}

		until begin() const
		{
			return until(i, p);
		}
		until end() const
		{
			return until(upto(i, p), p);
		}

		explicit operator bool() const
		{
			return !p(i);
		}
		value_type operator*() const
		{
			return *i;
		}
		until& operator++()
		{
			if (operator bool()) {
				++i;
			}

			return *this;
		}
		until operator++(int)
		{
			until u_(i, p);
			operator++();

			return u_;
		}
	};


#ifdef _DEBUG

	template<input_iterable I>
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

	template<input_iterable I>
	inline constexpr size_t length(I i, size_t n = 0)
	{
		while (i++)
			++n;

		return n;
	}
#ifdef _DEBUG
	template<input_iterable I, input_iterable J>
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
	template<input_iterable I, input_iterable J>
	inline J& copy(I i, J& j)
	{
		while (i and j) {
			*j++ = *i++;
		}

		return j;
	}

	// t, t + dt, ...
	template<class T, class dT = T>
	class sequence {
		T t;
		dT dt;
	public:
		using iterator_category = std::input_iterator_tag; // ??? bidirectional
		using difference_type = dT;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

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
			return sequence(((dt > 0) - (dt < 0))*std::numeric_limits<T>::max(), dt);
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
				using std::chrono::year_month_day;
				using std::chrono::months;
				using std::chrono::year;
				constexpr auto b{ year{2001} / 1 / 1 };
				// sequence s2(b, months{ 3 }); // this is why people hate chrono
				sequence<year_month_day,months> s(b, months{ 3 });

				assert(s);
				auto s2{ s };
				assert(s2);
				assert(s == s2);
				s = s2;
				assert(!(s != s2));
				++s;
				assert((*s).year() == std::chrono::year(2001));
				assert((*s).month() == std::chrono::month(4));
				assert((*s).day() == std::chrono::day(1));
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
		using iterator_category = std::input_iterator_tag; //!!! random access
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

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
		reference operator*()
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
			ptr p_{*this};
			operator++();

			return p_;
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

	template<input_iterable I>
	class take {
		size_t n;
		I i;
	public:
		using iterator_category = typename I::iterator_category;
		using difference_type = typename I::difference_type;
		using value_type = typename I::value_type;
		using pointer = typename I::pointer;
		using reference = typename I::reference;

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
			{
				typename I::value_type p[] = { 1,2,3 };
				auto t = take(3, ptr(p));
				assert(3 == *back(t));
			}

			return 0;
		}
	#endif // _DEBUG
	};

	template<input_iterable I>
	inline I drop(size_t n, I i)
	{
		while (n--)
			++i;

		return i;
	}

	// keep track of iterable increments
	template<input_iterable I>
	class counted {
		I i;
		size_t n;
	public:
		using iterator_category = typename I::iterator_category;
		using difference_type = typename I::difference_type;
		using value_type = typename I::value_type;
		using pointer = typename I::pointer;
		using reference = typename I::reference;

		counted(I&& i, size_t n = 0)
			: i(std::move(i)), n(n)
		{ }
		counted(const counted&) = default;
		counted& operator=(const counted&) = default;
		~counted()
		{ }

		I& iter()
		{
			return i;
		}
		const I& iter() const
		{
			return i;
		}
		size_t count() const
		{
			return n;
		}

		auto operator<=>(const counted&) const = default;

		counted begin() const
		{
			return *this;
		}
		counted end() const
		{
			return counted(i, length(i));
		}

		explicit operator bool() const
		{
			return !!i;
		}
		value_type operator*() const
		{
			return *i;
		}
		counted& operator++()
		{
			if (operator bool()) {
				++n;
				++i;
			}

			return *this;
		}
		counted operator++(int)
		{
			counted t_{ *this };
			operator++();

			return t_;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				typename I::value_type p[] = { 1,2,3 };
				auto t = counted(ptr(p));
				assert(t);
				assert(0 == t.count());

				auto t2{ t };
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
				assert(1 == t.count());
				assert(2 == *t);

				assert(2 == *t++);
				assert(2 == t.count());
				assert(3 == *t);
			}

			return 0;
		}
#endif // _DEBUG
	};

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

	template<input_iterable I, input_iterable J>
	class pair {
		I i;
		J j;
	public:
		using iterator_category = std::common_type_t<typename I::iterator_category, typename J::iterator_category>;
		using difference_type = ptrdiff_t; // ???
		using value_type = std::pair<typename I::value_type, typename J::value_type>;
		using pointer = value_type*;
		using reference = value_type&;

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

	template<class F, input_iterable I,
		class X = typename I::value_type, class Y = std::invoke_result_t<F, X>>
	class apply {
		const F& f;
		I i;
	public:
		using iterator_category = typename I::iterator_category;
		using difference_type = typename I::difference_type;
		using value_type = Y;
		using pointer = value_type*;
		using reference = value_type&;

		apply(const F& f, const I& i)
			: f(f), i(i)
		{ }

		bool operator==(const apply& a) const
		{
			return &f == &a.f and i == a.i;
		}

		apply begin() const
		{
			return apply(f, i);
		}
		apply end() const
		{
			return apply(f, i.end());
		}

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
		apply operator++(int)
		{
			apply a_(f, i);
			operator++();

			return a_;
		}
	};
#ifdef _DEBUG
	inline int test_apply()
	{
		{
			apply a([](int i) { return i * i; }, sequence<int>(-2));
			assert(a);
			assert(4 == *a);
			assert(1 == *++a);
			assert(0 == *++a);
			assert(1 == *++a);
		}
		{
			apply a(std::negate<int>{}, sequence<int>(-2));
			assert(2 == *a);
			assert(1 == *++a);
			assert(0 == *++a);
			assert(-1 == *++a);
		}

		return 0;
	}
#endif // _DEBUG

	template<class F, input_iterable I,
		class T = typename I::value_type>
	class fold {
		const F& f;
		I i;
		T t;
	public:
		using iterator_category = typename I::iterator_category;
		using difference_type = typename I::difference_type;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;

		fold(const F& f, const I& i, const T& t0)
			: f(f), i(i), t(t0)
		{ }

		bool operator==(const fold& a) const
		{
			return &f == &a.f and i == a.i and t == a.t;
		}

		fold begin() const
		{
			return fold(f, i, t);
		}
		fold end() const
		{
			return fold(f, i.end(), t);
		}

		explicit operator bool() const
		{
			return !!i;
		}
		auto operator*() const
		{
			return t;
		}
		fold& operator++()
		{
			++i;

			return *this;
		}
		fold operator++(int)
		{
			fold a_(f, i, t);
			operator++();

			return a_;
		}
	};

	// make iterable from container
	template<class C>
	class container {
		C::iterator b, e;
	public:
		using iterator_category = typename C::iterator::iterator_category;
		using difference_type = typename C::iterator::difference_type;
		using value_type = typename C::iterator::value_type;
		using pointer = typename C::iterator::pointer;
		using reference = typename C::iterator::reference;

		// user responsible for container lifetime
		container(C& c)
			: b(c.begin()), e(c.end())
		{ }
		container(const container&) = default;
		container& operator=(const container&) = default;
		~container()
		{ }

		bool operator==(const container& c) const
		{
			return equal(*this, c);
		}

		container begin() const
		{
			return *this;
		}
		container end() const
		{
			container c_{ *this };
			c_.b = c_.end();

			return c_;
		}

		explicit operator bool() const
		{
			return b != e;
		}
		value_type operator*() const
		{
			return *b;
		}
		reference operator*()
		{
			return *b;
		}
		container& operator++()
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
		container operator++(int)
		{
			auto c_{ *this };
			operator++();

			return c_;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				C c = { 1,2,3 };
				container i(c);
				assert(i);
				auto i2{ i };
				assert(i2);
				i = i2;
				assert(i == i2);
				assert(1 == *i);
				++i;
				assert(2 == *i);
				assert(2 == *i++);
				assert(3 == *i);
				assert(!++i);
			}

			return 0;
		}
#endif // _DEBUG
	};

} // namespace fms
