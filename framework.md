---
title: Framework
author: Keith A. Lewis
institution: KALX, LLC
email: kal@kalx.net
classoption: fleqn
fleqn: true
abstract: implementing financial mathematics
...

\newcommand\RR{\bm{R}}
\newcommand\NN{\bm{N}}
\newcommand{\Var}{\operatorname{Var}}

# PolyFin

Some aspects of the financial world can be modeled using mathematics,
but ultimatley the people writing checks need that implemented on
a computer to produce numbers they find useful for running their business.
Students who can demonstrate they have the skills to do this are
students who get hired.

## Library

Terminology: _Price_ is the market price of an instrument. _Value_ is
an analog of price computed using a model.

Distrbutions. Cumulative distribution functions random variables
and their derivatives required for option valuation and greeks.

Root finding. 1d Hewlett-Packer/Kahane Solver. It just works.

AD. Automatic differentiation using [ε](https://github.com/keithalewis/epsilon).
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

Discount. The price of a zero coupon bond maturing at $t$ is $D(t) = \exp(-\int_0^t f(s)\,ds)$
where $f(s)$ is the continuously compounded forward rate at $s$.
The _spot rate_ $r$ is defined by $D(t) = \exp(-t r(t))$ so
$r(t) = (1/t)\int_0^t f(s)\,ds$ is the average forward rate.

European option valuation. 
The (spot) value of an European option paying $ν(F)$ at expiration $t$ is $v = D(t)E[ν(F)]$.
Greeks can be evaluated using the chain rule.

Bootstrap. Use piecewise flat forward curve.
Bond yield is a special case for a single fixed income instrument using a constant forward.
Interpolate by adding synthetic instruments that make sense to traders, not by using splines.

Stochatic rates. Equity volatility swamps out interest rate volatility, but the
stochastic forward rate determines the value of every fixed income instrument.

Deflator. Reciprocal of money market account where one unit is reinvested at
the prevailing repo rate $f_s$: $D_t = \exp(-\int_0^t f_s\,ds)$.

FTAP: Every arbitrage-free model is parameterized by a vector-valued martingale
and a deflator.

Zero. $D_t(u) = E_t[D_u]/D_t = E_t[\exp(-\int_t^u f(s)\,ds)] = \exp(-\int_t^u f_t(s)\,ds)$.

Valuation. A derivative is a contract paying $A_j$ at $τ_j$. $V_t = E_t[\sum_{τ_j>t} A_j D_{τ_j}]/D_t$.

Binomial Model. It is Brownian motion in the limit. The increase in computing power
makes it more practical.

Trinomial Model. More efficent than binomial. Use the same valuation code.

Fixed Income. Daycount fractions. Valuation versus settlement date.

Risky Fixed Income. Default time and recovery of bond issuer.

LIBOR Market Model. Convexity adjustment for forwards versus futures.
Parameterized by instantaneous futures, atm caplets, and
instantaneous futures/forward correlation. Correlations do not change discount and caplet prices.

Variance Swaps. Model independent valuation. Cubic term explains P&L, not delta or other greeks.

## Desiderata

_Prefer functions to objects._

Functions have no side effects and only return a result.
This enables compilers to generate efficient code since they have no side
effects that might mutate data.
Functions, by definition, are composable
and make refactoring simpler.
You know exactly what a functions does by looking at the code.

Objects have data and methods, functions that can use and change object data.
They map more closely to natural language usage when writing code.
Object inheritance can reduce the amount of code by implementing common operations.
Objects are difficult to design well and impede refactoring.

_Prefer streams to memory allocation._

Streams produce data on demand and don't care where it comes from.
They generalize in-memory arrays that are accessed sequentially.
This allows for potentially infinite streams and generators that
run asynchronously.

_Use coroutines for asynchronous code execution._

A coroutine is a function with a bookmark. Functions execute statements
and return a value. Coroutines execute statements that can yield a value
and set a bookmark. The next call to the coroutine resumes execution
from the bookmark until the next yield or a return that terminates
the coroutine. They are the simplest way to implement cooperative multi-tasking.

Implement core in C++. Provide interfaces to other languages.  
.Net. [C++/CLI](https://docs.microsoft.com/en-us/cpp/dotnet/dotnet-programming-with-cpp-cli-visual-cpp)  
Python. [CFFI](https://cffi.readthedocs.io/en/latest/),
[Cython](https://cython.org/),
[SIP](https://www.riverbankcomputing.com/software/sip/intro).  
Javascript. [Emscripten](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/Emscripten)

<!--
## Finance

Market _instruments_ have _prices_ and _cash flows_.

Instruments _trade_ in units of _amount_. For example, stocks trade in shares,
bonds in terms of notional, exchange traded instruments specify contract size.

Each instrument _issuer_ determines its associated cash flows.

A _holding_ is an amount, instrument, and who owns it. Holders get
cash flows proportional to the amount held.

Trades result in _positions_: a collection of holdings.

A _trade_ is an exchange of holdings at some time between a _buyer_ and a _seller_.

Buyers decide when, what, and how much to trade. Sellers quote prices to buyers.

The price for a trade (after it has _settled_) is the ratio of buyer amount and seller amount.
It might not be the same as the quoted price (_slippage_).

Dynamic trading creates synthetic market instruments.
The values (mark-to-market) and amounts (trade blotter entries)
correspond to prices and cash flows, respectively, of the synthetic instrument.

## Mathematics

A _model_ specifies prices $X_t$ and cash flows $C_t$ for instruments
depending on what happens in the future. These are random
variables indexed by time: a stochastic process.

A _sample space_ $Ω$ is the set of what can happen in the future.
Partial information 
is specified by a _partition_ $\mathcal{A}$ of the sample space.
A function $X\colon Ω\to\bm{R}$ is $\mathcal{A}$-measurable if
it is constant on each element of the partition.
In this case $X\colon\mathcal{A}\to\bm{R}$ **is** a function on
the elements of the partition.

Let $E$ be the collection of all _entities_ (market participants),
$I$ be the collection of all instruments, and $A_i$, $i\in I$ be
the amounts at which instrument $i$ can trade. A _holding_ is
a triple $(a,i,e)$ where $e\in E$ is the entity owning amount
$a\in A_i$ of instrument $i\in I$. A _trade_ $(t;a,i,e;a',i',e')$
indicates buyer $e$ exchanged $(a,i,e)$ for $(a',i',e')$ with
seller $e'$. After the trade the buyer holds $(a',i',e)$ and
the seller holds $(a,i,e')$.

A _position_ is a collection of holdings. The _net ammount_ of is the
sum of the amounts of all holdings in the position for a given instrument
and entity. This assumes instruments are _fungible_: every
entity can convert $(a,i)$ to $(a_0,i)$ and $(a - a_0,i)$ at no cost.
This is close to being true if $a > a_0 > 0$.

A _trading strategy_ is increasing sequence of times and amounts of market instruments to trade.
It has associatd _values_ and _amounts_
Let $τ_j$ be the trading times and $Γ_j$ be the amount of
shares to trade in each instrument. Trades accumulate to
a _position_ $Δ_t = \sum_{τ_j < t} Γ_j$. The value of
the strategy at time $t$ is $V_t = (Δ_t + Γ_t)\cdot X_t$.
The amounts associated with a strategy at time $t$ are
$A_t = Δ_t\cdot C_t - Γ_t\cdot X_t$. They correspond to
the mark-to-market and entries in the trade blotter, respectively,
for the strategy.

## Implementation

Time corresponds to a datetime type, often in terms of UTC.
The difference of two times is an _interval_ or _period_.
A time and interval can be added to produce a time.
We have $t_1 = t_0 + (t_1 - t_0)$ for times $t_0$, $t_1$ and
interval $t_1 - t_0$. It is common to fix an _epoch_ $t_0$
and express time $t$ as an interval $t - t_0$ in units of years.

Price and cash flow are functions from $T\times \Omega\to\bm{R}^I$,
where $I$ is the set of instruments.

### Iterable

An iterable $i$ of type $T$ has three functions:
`curr:I -> T` giving its current value,
`next:I -> I` that removes the current value,
and `done:I -> Bool` indicating the iterable has no more values available.

## Usage

Examples of usage in a pseudo language:

```
	// At time t0 stock has price s0 and optional dividend schedule.
	stock S(t0, s0[, d0])
	// Geometric Brownian motion with volatility σ.
	model::bsm M(σ)
	// Put option with strike k and expiration t
	option::put p(k, t)
	// Value is a function of time and state given market instruments and a model
	v = value(p, S, M)(t0, ω0) 

```

Coding conventions

```
    namespace xyz {
    	class C {
	public:

	    static const char* doc = R"()";
	    C()
	    { }

	    static const char* foo_doc = R"()";
	    int foo(int)
	    {
	    }
	};
    }
```
-->
