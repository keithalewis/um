---
title: PolyFin
author: Keith A. Lewis
institution: KALX, LLC
email: kal@kalx.net
classoption: fleqn
fleqn: true
abstract: Software implementation of financial mathematics.
...

\newcommand\RR{\bm{R}}
\newcommand\NN{\bm{N}}
\newcommand{\Var}{\operatorname{Var}}

An adaptable toolkit for financial modeling.

Some aspects of the financial world can be modeled using mathematics,
but ultimately the people writing checks need that implemented on
a computer to produce numbers they find useful for running their business.
Students who can demonstrate they have the skills to do this are
students who get hired.

The PolyFin core library is written using C++20 and includes language bindings for
Python using [CFFI](https://cffi.readthedocs.io/en/latest/), [Cython](https://cython.org/),
[SIP](https://www.riverbankcomputing.com/software/sip/intro),
.Net using [C++/CLI](https://docs.microsoft.com/en-us/cpp/dotnet/dotnet-programming-with-cpp-cli-visual-cpp),
and Javascript using [Emscripten](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/Emscripten).
It uses functions instead of objects when feasible. 
Functions have no side effects and only return a result. 
This enables compilers to generate efficient code since they have no
side effects.  Functions, by definition, are
composable and make it easier to integrate PolyFin into other systems.

PolyFin also uses stream oriented interfaces that avoid memory allocation.
Streams produce data on demand and don't care where it comes from.
They generalize in-memory arrays that are accessed sequentially.
This allows for potentially infinite streams that can
be implemented using coroutines.

A coroutine is a function with a bookmark.
Functions execute statements and return a value.
Coroutines execute statements that can yield a value and set a bookmark.
Subsequent calls to the coroutine resume execution from the bookmark until
the next yield, or a return that terminates the coroutine.
They are the simplest way to correctly implement cooperative multi-tasking.

## Library

Terminology: _Price_ is the market price of an instrument. _Value_ is
an analog of price computed using a model.

Distributions. Cumulative distribution functions of random variables
and derivatives required for option valuation and greeks.

Root finding. 1d Hewlett-Packard/Kahane Solver. It just works.

Automatic differentiation. Use [$\epsilon$](https://github.com/keithalewis/epsilon).
Also useful for Machine Learning.

European option (forward/Black) valuation. $F = f \exp(s X - κ(s))$ parameterizes all positive payoffs
by $f$, $s$, and $X$ where $E[X] = 0$ and $\Var(X) = 1$. Note $E[F] = f$, $s^2 = \Var(\log F)$
and $κ(s) = \log E[\exp(s X)]$ is the cumulant generating function.
The forward value of an European option paying $ν(F)$ at expiration is $v = E[ν(F)]$.
For a put option
$$
	v = E[\max\{k - F,0\}] = E[(k - F)1(F \le k)] = kP(F\le k) - f P_s(F\le k),
$$
where $dP_s/dP = \exp(s X - κ(s))$ is share/Esscher measure.
All greeks can be computed in terms of the derivatives of $P_s(X\le X)$ and $κ(s)$.
The Carr-Madan formula extends this to any payoff function.
The Black model is the case $X = B_t/\sqrt{t}$ and $s = \sigma\sqrt{t}$,
where $B_t$ is standard Brownian motion.

Discount. The price of a zero coupon bond maturing at $t$ is $D(t) = \exp(-\int_0^t f(s)\,ds)$
where $f(s)$ is the continuously compounded forward rate at $s$.
The _spot rate_ $r$ is defined by $D(t) = \exp(-t r(t))$ so
$r(t) = (1/t)\int_0^t f(s)\,ds$ is the average forward rate.

European option valuation. Cost of carry: spot $S_t = F_tD(t)$.
The (spot) value of an European option paying $ν(S_t)$ at expiration $t$ is $v = D(t)E[ν(F_t/D(t))]$.
Greeks can be evaluated using the chain rule.

Bootstrap. Use a piece-wise flat forward curve.
Bond yield is a special case for a single fixed income instrument using a constant forward.
Interpolate by adding synthetic instruments that make sense to traders, not by using splines.

Stochastic rates. Deterministic rates can be used for equities since
equity volatility swamps out interest rate volatility.
The stochastic forward rate determines the value of every fixed income instrument.

Deflator. Reciprocal of money market account where one unit is reinvested at
the prevailing repo rate $f_s$: $D_t = \exp(-\int_0^t f_s\,ds)$.

FTAP: Every arbitrage-free model is parameterized by a vector-valued martingale
$M_t$ and a deflator where the deflated price is
$$
	X_t D_t = M_t - \sum_{s\le t} C_s D_s.
$$
Prices $X_t$ and cash flows $C_t$ satisfy
$$
	X_t D_t = E_t[X_u D_u + \sum_{t < s \le u} C_s D_s], u > t.
$$

Zero Coupon Bond. The price at time $t$ of a zero coupon bond maturing at $u$ is
$$
	D_t(u) = E_t[D_u]/D_t = E_t[e^{-\int_t^u f(s)\,ds}] = e^{-\int_t^u f_t(s)\,ds},
$$
where $f_t$ is the forward curve at time $t$.

Valuation. A derivative is a contract paying $A_j$ at times $τ_j$.
In an arbitrage-free model its value is $V_t = E_t[\sum_{τ_j>t} A_j D_{τ_j}]/D_t$.

Binomial Model. Not just for MBA's anymore. It is Brownian motion in
the limit. The increase in computing power makes it practical to use.

Trinomial Model. More efficient than binomial. Uses the same valuation code.

Fixed Income. Instruments paying fixed cash flows.  Valuation versus settlement date.
Daycount fractions.  Specify contacts generically (ISDA agreements).
Valuation date and price/par coupon fix cash flows when trade occurs.

Risky Fixed Income. The default time of and recovery from bond issuer determine dynamics.
Credit spread incorporates both in a single number.

LIBOR Market Model. Convexity adjustment for forwards versus futures.
Parameterized by futures, at-the-money caplets, and
futures correlations. Correlations do not change discount and caplet prices.

Variance Swaps. Model independent valuation. Cubic term explains P&L, not delta or other greeks.

## Design

Streams, random variables and conditional expection, coroutines...

Generic framework for valuing any instrument.

## Sample Usage

### Option Pricing and Greeks

```
	// generalized logistic distribution
	logistic L(0.1, 0.2);

	// stock with price 100 and annual vol 20%
	stock S(100, 0.2);

	// three month 100 strike call
	call C(100, 0.25);   

	double v = value(C, S, L);   // option value
	double dv = delta(C, S, L);  // option delta

	// implied volatility given option price
	double s = implied(p, C, S, L);
```

### Bootstrap

```
	vector<fixed_income_instrument> is;

	// T+1 one week cash deposit
	is.emplace_back(cash_deposit(day(1), week(1), dcf_actual_360));
	...
	// T+3 10 year fixed-float swap
	is.emplace_back(fixed_float_swap(day(3), year(10), dcf_30_360, dcf_actual_360));

	// corresponding instrument coupons
	vector<double> cs = {0.01,..., 0.02};

	pwflat::curve c;
	date valuation(2021/1/1);
	for (auto [i,c] : pair(is, cs)) {
		i.fix(valuation, c); // determines actual cash flows
		c.push_back(bootstrap(c, i));
	}
	// use c...

```

### Barrier Option

```
	binomial B(dt);
	stock S(S0, vol);
	up_in_call UIC(h, k, t);
	double v = value(UIC, S, B);

	// UIC implementation
	// first time barrier is exceeded
	auto tau = take(1, max(S(B.times())) > UIC.h);
	// cash flows are functions T -> Omega -> R
	// A = t -> (w -> max(S(t)(w) - k, 0))
	auto A = [S,UIC](auto t) {
		return [S,UIC](auto w) {
			return max(S(t)(w) - UIC.k, 0);
		};
	};

```

## Remarks

State of the art practices.
  - Build system, continuous integration, code coverage, 
