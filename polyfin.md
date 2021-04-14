---
title: PolyFin
author: Keith A. Lewis
institution: KALX, LLC
email: kal@kalx.net
classoption: fleqn
fleqn: true
abstract: Software implementation of financial mathematics
...

\newcommand\RR{\bm{R}}
\newcommand\NN{\bm{N}}
\newcommand{\Var}{\operatorname{Var}}

# PolyFin 

An adaptable toolkit for financial modeling.

Some aspects of the financial world can be modeled using mathematics,
but ultimately the people writing checks need that implemented on
a computer to produce numbers they find useful for running their business.
Students who can demonstrate they have the skills to do this are
students who get hired.

The core library is written using C++20 and includes language bindings
Python using [CFFI](https://cffi.readthedocs.io/en/latest/), [Cython](https://cython.org/),
.Net using [C++/CLI](https://docs.microsoft.com/en-us/cpp/dotnet/dotnet-programming-with-cpp-cli-visual-cpp),
[SIP](https://www.riverbankcomputing.com/software/sip/intro),  
and Javascript using [Emscripten](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/Emscripten).
It uses functions instead of objects when feasible. 
Functions have no side effects and only return a result. 
This enables compilers to generate efficient code since they have no
side effects.  Functions, by definition, are
composable and make refactoring simpler.

PolyFin also uses stream oriented interfaces that avoid memory allocation.
Streams produce data on demand and don't care where it comes from.
They generalize in-memory arrays that are accessed sequentially.
This allows for potentially infinite streams that can
be implemented using coroutines.

A coroutine is a function with a bookmark.
Functions execute statements and return a value.
Coroutines execute statements that can yield a value and set a bookmark.
The next call to the coroutine resumes execution from the bookmark until
the next yield, or a return that terminates the coroutine.
They are the simplest way to correctly implement cooperative multi-tasking.

## Library

Terminology: _Price_ is the market price of an instrument. _Value_ is
an analog of price computed using a model.

Distributions. Cumulative distribution functions of random variables
and derivatives required for option valuation and greeks.

Root finding. 1d Hewlett-Packer/Kahane Solver. It just works.

AD. Automatic differentiation using [\epsilon](https://github.com/keithalewis/epsilon).
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
The Black model is the case where $X = B_t/\sqrt{t}$ and $s = \sigma\sqrt{t}$,
where $B_t$ is standard Brownian motion.

Discount. The price of a zero coupon bond maturing at $t$ is $D(t) = \exp(-\int_0^t f(s)\,ds)$
where $f(s)$ is the continuously compounded forward rate at $s$.
The _spot rate_ $r$ is defined by $D(t) = \exp(-t r(t))$ so
$r(t) = (1/t)\int_0^t f(s)\,ds$ is the average forward rate.

European option valuation. 
The (spot) value of an European option paying $ν(F)$ at expiration $t$ is $v = D(t)E[ν(F)]$.
Greeks can be evaluated using the chain rule.

Bootstrap. Use piece-wise flat forward curve.
Bond yield is a special case for a single fixed income instrument using a constant forward.
Interpolate by adding synthetic instruments that make sense to traders, not by using splines.

Stochastic rates. Equity volatility swamps out interest rate volatility, but the
stochastic forward rate determines the value of every fixed income instrument.

Deflator. Reciprocal of money market account where one unit is reinvested at
the prevailing repo rate $f_s$: $D_t = \exp(-\int_0^t f_s\,ds)$.

FTAP: Every arbitrage-free model is parameterized by a vector-valued martingale
and a deflator.

Zero. The price at time $t$ of a zero coupon bond maturing at $u$ is
$$
	D_t(u) = E_t[D_u]/D_t = E_t[e^{-\int_t^u f(s)\,ds}] = e^{-\int_t^u f_t(s)\,ds},
$$
where $f_t$ is the forward curve at time $t$.

Valuation. A derivative is a contract paying $A_j$ at $τ_j$.
In an arbitrage-free model its value is $V_t = E_t[\sum_{τ_j>t} A_j D_{τ_j}]/D_t$.

Binomial Model. It is Brownian motion in the limit. The increase in computing power
makes it practical to use these days.

Trinomial Model. More efficient than binomial. Use the same valuation code.

Fixed Income. Daycount fractions. Valuation versus settlement date.
Specify contacts generically (like ISDA) and use valuation date and price/par coupon
to fix cash flows.

Risky Fixed Income. Default time and recovery of bond issuer determine dynamics.

LIBOR Market Model. Convexity adjustment for forwards versus futures.
Parameterized by futures, at-the-money caplets, and
futures correlations. Correlations do not change discount and caplet prices.

Variance Swaps. Model independent valuation. Cubic term explains P&L, not delta or other greeks.

## Design

Streams, random variables and conditional expection, coroutines...

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
		i.fix(valuation, c); // set coupon
		c.push_back(bootstrap(c, i));
	}

```

### Barrier Option

```
	binomial B(dt);
	stock S(S0, vol);
	up_in_call UIC(h, k, t);
	double v = value(UIC, S, B);

	// UIC implementation
	auto tau = take(1, max(S(B.times())) > UIC.h);
	// cash flows are functions T -> Omega -> R
	auto A = [S,UIC](auto t) {
		return [S,UIC](auto w) {
			return max(S(t)(w) - UIC.k, 0) * (max(S)(t)(w) > UIC.h);
		};
	};

```
