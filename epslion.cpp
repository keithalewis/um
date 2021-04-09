// epsilon.cpp - automatic differentiation
#include <cassert>
#include <algorithm>
#include <valarray>
#include <cstddef>
#include <initializer_list>

// x e^k
template<size_t N, class X = double>
class epsilon {
	std::valarray<X> x; // x[0] + x[1] e + ... + x[n-1]/(n - 1)! e^{n - 1}
public:
	epsilon()
		: x(N)
	{
	}
	epsilon(const std::initializer_list<X>& x)
		: x(x)
	{
		assert(x.size() == N);
	}
	epsilon(const epsilon&) = default;
	epsilon& operator=(const epsilon&) = default;
	epsilon(epsilon&&) = default;
	epsilon& operator=(epsilon&&) = default;
	~epsilon()
	{ }

	X& operator[](size_t n)
	{
		return x[n];
	}
	const X& operator[](size_t n) const
	{
		return x[n];
	}

	// scalars
	epsilon& operator+(const X& c)
	{
		x[0] += c;

		return *this;
	}
	epsilon& operator-(const X& c)
	{
		x[0] -= c;

		return *this;
	}
	epsilon& operator*(const X& c)
	{
		x *= c;

		return *this;
	}
	epsilon& operator/(const X& c)
	{
		x /= c;

		return *this;
	}

	epsilon& operator+(const epsilon& y)
	{
		x += y.x;

		return *this;
	}
	epsilon& operator-(const epsilon& y)
	{
		x -= y.x;

		return *this;
	}
	// (sum_j x[j]/j! e^j)(sum_k y[k]/k! e^k) = sum_n sum_{j + k = n} C(j,k) x[j]y[k]/n! e^n
	epsilon& operator*(const epsilon& y)
	{
		std::valarray<X> z(N);

		for (size_t n = 0; n < N; ++n) {
			X Cnk = 1;
			for (size_t k = 0; k <= n; ++k) {
				z[n] += Cnk*x[k]*y.x[n-k];
				Cnk *= n - k;
				Cnk /= k + 1;
			}
		}
		std::swap(x,z);

		return *this;
	}
	epsilon& operator/=(const epsilon& y)
	{
		std::valarray<X> z(N);
		X y0 = y[0];
		for (size_t n = 0; n < N; ++n) {
			X Cnk = 1;
			z[n] = x[n];
			for (size_t k = 0; k < n; ++k) {
				z[n] -= Cnk*y.x[k]*z[n-k];
				Cnk *= n - k;
				Cnk /= k + 1;
			}
			z[n] /= y0;
		}
		std::swap(x, z);

		return *this;
	}
};

int main()
{
	return 0;
}
