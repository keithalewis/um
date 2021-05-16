---
title: Financial Markets
classoption: fleqn
fleqn: true
...

\newcommand\bm{\mathbf}
\renewcommand\implies{\Rightarrow}
\newcommand\RR{\bm{R}}
\newcommand\st{\mid}
\newcommand{\Var}{\operatorname{Var}}

Participants (legal _entities_) in financial markets hold _amounts_ of _securities_.
_Securities_ are tangible assets such as stocks, bonds, currencies, and commodities.
[Introduce derivatives later.]
Securities _trade_ in specified amounts: shares, notional, denominations, and 
physical quantities, respectively.
_Buyers_ decide when and how much of each security to trade
based on the price offered by _sellers_, among other considerations.
Trades occur _over-the-counter_ or on _exchanges_.

Over-the-counter refers to trades between a buyer and a seller.
Exchanges have liquidity providers who _make a market_
for securities. They quote _prices_ to customers
at which they can buy or sell given _amounts_.
Exchanges often do not hold instruments; they intermediate
between buyers and sellers and extract a fraction of the amounts exchanged
for the service they provide.
_Brokers-Dealers_ also facilitate transactions between market participants.
They hold inventories of securities that are subject to
market price movements so they are subject to more risk than exchanges.
[No exchange has every gone bankrupt. Global Crossing?]

Holding an amount of an instrument entitles the owner to all _cash flows_ associated with the instrument:
dividends, coupons, margin adjustments, and storage costs for
stocks, bonds, futures, and commodities respectively.
Currencies do not have cash flows. Cash flows are proportional to
the amount of a security held and are determined by the issuer of the instrument.
Holdings are exchanged by _trades_ between a buyer and a seller
and accumulate to a position in instruments for the legal entities involved.

A _derivative_ is a contract between a buyer and a seller for the
exchange of future cash flows. As with any contract, they are subject
to the laws and regulations of the governing country.
Derivatives have been used throught human history to enable humans to
benefit from what Adam Smith called 'division of labor.'
If Korg was good at catching bears but Grok was better at making arrows
they could make a benefical exchange. The introduction of _currency_
streamlined such trades.  Derivatives involve promises of
future exchanges. If Korg gives Grok a bear skin today for a quiver of
arrows at the next full moon can he trust Grok to deliver?
_Risk Management_ was a concern even before the first derivative transaction.

## Instrument

An _instrument_ is either a security or a derivative.

Security - tangible asset.

Derivative - contract between entities.

## Amount

Instruments trade in discrete amounts:
shares, notional, contracts, denominations, and quantity
for stocks, bonds, futures, currencies, and commodities respectively.

## Holding

A holding is an amount of an instrument owned by an entity.

## Trade

Exchange of holdings between entities at a given time.

## Position

Collection of holdings.

## Accounting

Net amount of instruments held by an entity.

# Mathematics

Math involves manipulating _propositions_, statements that are either true or false.
Propositions can be combined using _logical connectives_,
_not_ ($\neg$), _and_ ($\wedge$), _or_ ($\vee$), and _implies_ ($\implies$).
An _axiom_ (for a given mathematical system) is a proposition that is assumed to be true.
A _proof_ of '$A$ implies $B$' assumes $A$ is true and deduces $B$ using axioms and rules of inference.
The rule of inference _modus ponens_ states if $P$ and $P\implies Q$ then $Q$.

An _expression_ is a well-formed formula from a set of symbols and rules.
A _derivation_ demonstrates equality of expressions using _substitution_ and _evaluation_.

### Propositional Calculus

A (well-formed) proposition is an expression involving (propositional) variables or any
combinations of expressions built from logical connectives.
A tautology is an expression that is true for all values of the variables it involves.
Every tautology can be proved using the axioms
$P\implies P$,
$P\implies(Q\implies P)$,
and $(P\implies(Q\implies R))\implies((P\implies Q)\implies(P\implies R))$
using modus ponens.

### First Order Logic

If a proposition $P(x)$ is parameterized over a (symbolic) variable
then $\forall x\,P(x)$ indicates the parameterized proposition is true
for all values represented by $x$ and $\exists x\,P(x)$ indicates there
is at least one value for $x$ making the proposition true.

## Set

The language of set theory is ε.

## Relation

A _relation_ on the sets $X$ and $Y$ is a subset $R\subseteq X\times Y$.
Write $xRy$ for $(x,y)\in R$.
The _right coset_ of $x\in X$ is $xR = \{y\in Y\st xRy\}$.
The _left coset_ of $y\in Y$ is $Ry = \{x\in X\st xRy\}$.

The _transpose_ of a relation is $R' = \{(y,x)\st xRy\}$. Note $R'' = R$.

The _composition_ of $R\subseteq X\times Y$ and $S\subseteq Y\times Z$ is
$SR = \{(x,z)\st xR\cap Rz\not=\emptyset\}$.

Symmetric, 

## Function

A _function_ is a relation where the _right coset_ $xR = \{y\in Y\st xRy\}$ is a singleton for all $x\in X$.
Write $R(x)$ for that unique element.

Let $B(X)$ be the bounded functions from $X$ to $\RR$.

### Lambda Calculus

A λ-term is a _variable_, _abstraction_, or _application_.
A variable is a symbol, an abstraction is $(x\to E)$ where $x$ is a variable
and $E$ is a λ-term, and an application is $(EF)$ where $E$ and $F$ are λ-terms.

The lambda calculus can represent every function that can be computed.

## Measure

A _measure_ is a set function $μ\colon \mathcal{P}(X)\to\RR$ where
$μ(E\cup F) = μ(E) + μ(F) - μ(E\cap F)$, $E,F\subseteq X$, and $μ(\emptyset) = 0$.
Measures don't count things twice and the measure of nothing is zero.
Let $ba(X)$ be the (finitely additive) measures on $X$.

## Duality

The set of linear functionals from $B(X)$ to $\RR$ is $B(X)^*$ and
is isomorphic to $ba(X)$. If $m\in B(X)^*$ define $μ\in ba(X)$ by
$μ(E) = m(1_E)$. If $μ\in ba(X)$ define $m(\sum_i a_i 1_{A_i}) = \sum_i a_i μ(A_i)$,
$a_i\in\RR$, $A_i\subseteq X$.

## Probability

Degree of belief.

Conditional probability: $P(A\mid B) = P(A\cap B)/P(B)$
so $P(A\cap B) = P(A\mid B)P(B) = P(B\mid A)P(A)$
and $P(A\mid B) = P(B\mid A)P(A)/P(B)$.

Law of total probability: $P(A\mid B) = \sum_n P(A\mid B\cap C_n) P(C_n\mid B)$
if $\cup_n C_n \supseteq B$ and $C_i\cap C_j = \emptyset$ if $i \not= j$.

Bayes theorem $P(A_i\mid B) = P(B\mid A_i)/(\sum_j P(B\mid A_j)P(A_i))$
if $A_j$ are pairwise disjoint.

### Random Variable

A _random variable_ is a variable, a symbol taking the place of a number
in equations and inequalities, with
additional information about the probability of the values it can have.

The _cumulative distribution function_ of $X$ is $F(x) = P(X\le x)$ determines a random variable.
It determines a measure $A\mapsto P(X\in A) = \int_A dF(x)$, $A\subseteq \RR$.

For any cdf $F$, $F(x) \le F(y)$ if $x < y$,
$\lim_{x\to -\infty}F(x) = 0$,
$\lim_{x\to +\infty}F(x) = 1$,
and $\lim_{n\to\infty} F(x + 1/n) = F(x)$ since $\cap_n (-\infty, x + 1/n] = (-\infty, x]$.
Any such function determines a cdf.

A random variable is _discretely distributed_ if
$x_i, p_i\in\RR$, $p_i > 0$, $\sum_i p_i = 1$, and  $P(X = x_i) = p_i$.

A random variable is _continuously distributed_ if $F = \int F'$.

Expected value $E[X] = \int x\,dF(x)$, $E[f(X)] = \int f(x)\,dF(x)$.

Variance $\Var(X) = E[(X - EX)^2] = E[X^2] - E[X]^2$.

Moment generating function $m(s) = E[e^{sX}] = \sum_{n=0}^\infty m_n s^n/n!$
where $m_n = E[X^n]$ is the $n$-th moment.

Cumulant generating function $κ(s) = \log E[e^{sX}] = \sum_{n=0}^\infty κ_n s^n/n!$
where $κ_n$ is the $n$-th _cumulant_. Note $κ(0) = 0$, $κ'(0) = E[X]$, and
$κ''(0) = \Var(X)$.

#### Examples

Bernoulli $P(X = 1) = p$, $P(X = 0) = 1 - p$, $0\le p \le 1$.

Binomial $P(X = k) = \binom{n}{k} p^k (1 - p)^{n-k}$, $0\le p \le 1$.

Two random variables $X,Y$ are determined by their _joint distribution_
$F(x,y) = P(X\le x, Y\le y)$.

A finite collection of random variables $(X_i)$ are determined by
$F(x_1,\ldots,x_n) = P(X_1\le x_1,\ldots,X_n\le x_n)$.

### Stochastic Process

A _stochastic process_ $(X_t)_{t\in T}$ is a collection of random variables indexed by time $T$.
It is determined by all finite joint distributions
$F_{t_1,\ldots,t_n}(x_1,\ldots,x_n) = P(X_1\le t_1, \ldots, X_n\le x_n)$.

#### Brownian Motion

# Software

## Executable

## Library

## Functional Programming

Lambda calculus

Curry-Howard-Lambek isomorphism.

$Ix = x$, $Kxy = x$, $Sxyz = xz(yz)$ [Note $xyz = x(y(z))$]

$X = x\implies (xS)K$.  $XXX = K$ and $XXXX = S$ by β-reduction.
