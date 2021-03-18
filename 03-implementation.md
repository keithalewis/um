# Implementation

## Algebra

The atoms of a finite algebra form a partition.

$\mathcal{A}\preceq \mathcal{B}$ iff $\mathcal{B}$ is a refinement of $\mathcal{A}$.

Given $A\in\mathcal{A}$ and $\mathcal{A}\preceq \mathcal{B}$ let $A|_\mathcal{B}$
be the atoms of $\mathcal{B}$ contained in $A$.

If $β$ is a measure on $\mathcal{B}$ define $β|\mathcal{A}$ by
$β(A) = \sum\{β(B):B\in A|_\mathcal{B}\}$.

A _filtration_ is a function from time to partitions. $t\mapsto A_t\in\mathcal{A}_t$.

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

