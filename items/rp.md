---
title: Reflection Principle
classoption: fleqn
fleqn: true
...

The _reflection principle_ establishes the joint distribution of a stochastic process
having independent symmetric increments with its running maximum and running minimum.

__Theorem__. _Let $X_n = Y_1 + \cdots + Y_n$ be a stochastic process where $(Y_i)$ are independent
and $Y_i$ has the same law as $-Y_i$ for all $i$, then_
$$
P(X_n \le x, \max_{i\le n} X_i \ge a) = 
P(X_n \le x, \max_{i\le n} X_i \ge a) + P(2a - X_n \le x,\max_{i\le n} X_i \ge a).
$$

It follows immediately that
$$
E[f(X_n)1(\max_{i\le n} X_i \ge a)] = 
E[f(X_n)1(\max_{i\le n} X_i \ge a)] + E[f(2a - X_n)1(\max_{i\le n} X_i \ge a)]
$$
for any measurable function $f$. Since $(-X_i)$ satisfies the hypotheses of the theorem
and replacing $a$ by $-a$, $x$ by $-x$, we have
$$
P(X_n \ge x, \min_{i\le n} X_i \le a) = 
P(X_n \ge x, \min_{i\le n} X_i \le a) + P(2a - X_n \ge x,\min_{i\le n} X_i \le a).
$$
determining the joint distribution with the running minimum.

Note
$$
\begin{aligned}
	P(X_n \le x, \max_{i\le n} X_i \ge a)
	&= P(X_n \le x, {\color{blue}X_n \ge a}, \max_{i\le n} X_i \ge a)
	 + P(X_n \le x, {\color{blue}X_n < a}, \max_{i\le n} X_i \ge a) \\
	&= P(X_n \le x, \max_{i\le n} X_i \ge a)
	 + P(X_n \le x, X_n < a, \max_{i\le n} X_i \ge a) \\
\end{aligned}
$$
since $X_n \ge a$ implies $\max_{i\le n} X_i \ge a$.
We use the reflection priciple to evaluate $P(X_n \le x, X_n < a, \max_{i\le n} X_i \ge a)$.

Let $I$ be the first index where $\max_{i\le I} X_i \ge a$ and note
$X^a_n = Y_1 + \cdots + Y_{I} - Y_{I+1} - \cdots - Y_n$ has the
same law as $(X_i)$ (the reflection principle).
We have $X^a_i + X_i = 2 X_I = 2a + 2ΔX$ for $i > I$
where $ΔX = X_I - a \ge 0$ is the overshoot at $I$.
If $ΔX = 0$ then
$$
\begin{aligned}
	P(X_n \le x, X_n < a, \max_{i\le n} X_i \ge a)
	&= P(X^a_n \le x, X^a_n < a, \max_{i\le n} X^a_i \ge a) \\
	&= P(2a - X_n \le x, 2a - X_n < a, \max_{i\le n} X^a_i \ge a) \\
	&= P(2a - X_n \le x, X_n > a, \max_{i\le n} X^a_i \ge a) \\
	&= P(2a - X_n \le x, \max_{i\le n} X^a_i \ge a) \\
\end{aligned}
$$
since $X_n > a$ implies $\max_{i\le n} X^a_i \ge a$.
The latter holds if and only if $\max_{i\le n} X_i \ge a$ so
this establishes the theorem in the case $ΔX = 0$.
Note this is the case if $(X_i)$ is a random walk and $a$ is an integer.

In general
$$
\begin{aligned}
	P(X_n \le x, X_n < a, \max_{i\le n} X_i \ge a)
	&= P(X^a_n \le x, X^a_n < a, \max_{i\le n} X^a_i \ge a) \\
	&= P(2a + 2ΔX - X_n \le x, 2a + 2ΔX - X_n < a, \max_{i\le n} X^a_i \ge a) \\
	&= \ldots \\
\end{aligned}
$$
