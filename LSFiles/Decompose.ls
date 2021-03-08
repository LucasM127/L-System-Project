#define C 5
#define L 0.2
Axiom:
A(0.05)

Skippable Letters:

Productions:
A(s) => A(s * C)
Decomposition Productions:
A(s) : s > L => A(s/2)A(s/2)
Homomorphic Productions:
A(s) => f(0.01)F(s - 0.02)f(0.01)
