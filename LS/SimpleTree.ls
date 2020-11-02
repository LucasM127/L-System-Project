#define C 2
#define L 1
Axiom:
A(0.3)

Skippable Letters:

Productions:
A(n) => B(n)[+(30)B(n)]
B(n) => A(n)[-(30)A(n)]
Decomposition Productions:
;A(s) : s > L => A(s/2)A(s/2)
Homomorphic Productions:
A(s) => f(0.05)F(s - 0.1)f(0.05)
B(s) => f(0.05)F(s - 0.1)f(0.05)
