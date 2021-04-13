// test.cpp
#include <cassert>
#include "fms_iterable.h"
#include "fms_pwflat.h"
#include "fms_root1d.h"
//#include "distribution.h"

using namespace fms;

/*
int test_when()
{
	{
		when w(sequence<int>(), [](auto i) { return *i % 2 == 0; });
		// sequence<int> s;
		// auto w = s | s % 2;
		assert(w);
		assert(0 == *w);
		++w;
		assert(2 == *w);
		assert(2 == *w++);
		assert(4 == *w);
	}

	return 0;
}
int test_when_ = test_when();
*/

int test_sequence_i = sequence<int>::test();
int test_sequence_d = sequence<double>::test();
int test_sequence_f = sequence<float>::test();

int test_ptr = ptr<int>::test();

int test_take = take<ptr<int>>::test();

int test_counted = counted<ptr<int>>::test();

int test_array_i = test_array<int>();

int test_length0 = test_length(take(2, sequence<int>()), take(3, sequence<int>()));
int test_length1 = test_length(take(0, sequence<int>()), take(3, sequence<int>()));
int test_length2 = test_length(take(0, sequence<int>()), take(0, sequence<int>()));

int test_value = pwflat::test_value();
//int test_root1d = secant<double,double>::test();

//int test_apply = apply<std::function<int(int)>,sequence<int>>::test();
int test_root1d = root1d::secant<double, double>::test();

int main()
{
	return 0;
}
