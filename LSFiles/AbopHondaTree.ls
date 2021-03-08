;pg 56 Modeling of trees ABOP pdf
#define r1 0.9 contraction ratio for trunk
#define r2 0.7 contraction ratio for branches
#define a0 30 branching angrle from the trunk
#define a2 -30 branching angle for lateral axes
#define D 137.5 divergence angle
#define wr 0.707 width decrease rate

Axiom:
A(1,0.2)

Skippable Letters:
Productions:
A(l,w) => !(w)X(l,w)[&(a0) B(l*r2,w*wr)]/(D)A(l*r1, w*wr)
B(l,w) => !(w)X(l,w)[-(a2)$C(l*r2,w*wr)]C(l*r1,w*wr)
C(l,w) => !(w)X(l,w)[+(a2)$B(l*r2,w*wr)]B(l*r1,w*wr)
Decomposition Productions:
Homomorphic Productions:
X(l,w) => F(l)
