\newcommand
# Implementation

## Partition

A _partition_ $\mathcal{A}$ of a set $Ω$ is a collection of disjoint subsets
of $Ω$ with union $Ω$. Partitions represent partial information:
we only know what element of the partition $ω\in Ω$ belongs to, not
$ω$ itself.
The coarsest partition $\{Ω\}$ represents no information. The finest
partition $\{\{ω\}:ω\in Ω\}$ represents complete information.

A partition are partially ordered by _refinement_:
$\mathcal{B}$ is a refinement of $\mathcal{A}$ if every
$B\in\mathcal{B}$ is a subset of some $A\in\mathcal{A}$.
Define $\mathcal{A}\preceq \mathcal{B}$ iff $\mathcal{B}$ is a refinement of $\mathcal{A}$.
We write $A|\mathcal{B}$ for the atoms in $\mathcal{B}$ contained in $A\in\mathcal{A}$.

## Algebra

An _algebra_ $\mathcal{A}^*$ on a set $Ω$ is a collection of subsets of $Ω$
closed under complement and union that contains the empty set.
This implies $Ω$ belongs to the algebra and that it is also closed
under intersection by De Morgan's laws.

An _atom_ of $\mathcal{A}^*$ is a set $A\in\mathcal{A}^*$ with the property
$B\subseteq A$ and $B\in\mathcal{A}^*$ imply $B = A$ or $B$ is the empty set.

The atoms of a finite algebra of sets on $Ω$ form a partition of $Ω$
and the algebra is the smallest algebra of sets containing the atoms.

It is simpler to work with partitions than with algebras and they are
equivalent in the finite case.

## Filtration

A _filtration_ on a totally ordered set $T$ is a collection of partitions $\mathcal{A}_t$, $t\in T$,
where $\mathcal{A}_t$ is coarser then $\mathcal{A}_u$ if $t < u$.

A _stopping time_ is a function $τ\colon Ω\to T$ such that
$\{ω\in Ω\mid τ(ω) = t\}$ is a union of atoms of $\mathcal{A}_t$.

A partition $\mathcal{B}$ is a _refinement_ of $\mathcal{A}$ if every
$B\in\mathcal{B}$ is a subset of some $A\in\mathcal{A}$.
Define $\mathcal{A}\preceq \mathcal{B}$ iff $\mathcal{B}$ is a refinement of $\mathcal{A}$.
If $T$ is totally ordered then $\mathcal{A}_T$ is a _filtration_ if
$\mathcal{A}_t\preceq\mathcal{A}_u$ when $t\le u$, $t,u\in T$.

A function $σ\colon Ω\to T$ is a _stopping time_ for $\mathcal{A}_T$ if
$σ^{-1}(t) = \{ω\in Ω:σ(ω) = t\}$ is a union of atoms belonging to $\mathcal{A}_t$.

## Instrument

An instrument returns a stream $(t, A)$ of times and amounts. $A$ is $t$ measurable.

$V_t = E_t[\sum_{u > t} A_u D_u]$.

$V_t(ω) =(\sum_{u > t} A_u D_u)(ω)$.

```
auto value(A, D)
{
	return [](t, ω) {
		while (A) {
			const auto& [u, a] = *A;
			if (u <= t) {
				++A;
			}

			return a(ω)*D(u,ω) + value(A, D)(ω);
		}

		return 0;
	};
}
```

$f\colon T\times R\to R$. $F(ω) = f(time(ω), ω(time(ω)))$.

Write example the works for both binomial and trinomial trees.

$atoms(ω,A)$ is a sequence of atoms

$E(X,P) = \sum \{ X(α) P(α) \mid α\in ω\}$.
