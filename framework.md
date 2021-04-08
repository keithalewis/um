# PolyFin

Some aspects of the financial world can be modeled using mathematics,
but ultimatley the people writing the checks need that implemented on
a computer to produce numbers they find useful for running their business.

## Desirata


_Prefer functions to objects._

Functions have no side effects and only return a result.  This enables
compilers to generate efficient code since they have no side
effects that might mutate data.  Functions, by definition, are composable.
You know exactly what they are doing by looking at the code

Objects have data and methods, functions that can use and change object data.
They map more closely to natural language usage when writing code.
Object inheritance can reduce the amount of code by implementing common operations.

_Prefer streams to memory allocation._

Streams produce data on demand and don't care where it comes from.
They generalize in-memory arrays that are only accessed sequentially.
This allows for potentially infinite streams and generators that
can leverage multicore CPUs.

_Use coroutines._

A coroutine is a function with a bookmark. Functions execute statements
and return a value. Coroutines execute statements that can yield a value
and set a bookmark. The next call to the coroutine resumes execution
from the bookmark until the next yield or a return that terminates
the coroutine.

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
