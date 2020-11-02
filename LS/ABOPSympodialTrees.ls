;pg 56 Modeling of trees ABOP pdf
#define r1 0.9 contraction ratio for trunk
#define r2 0.8 contraction ratio for branches
#define a1 35 branching angrle from the trunk
#define a2 35 branching angle for lateral axes
#define wr 0.707 width decrease rate

Axiom:
A(1,0.2)

Skippable Letters:
Productions:
A(l,w) => X(l,w)[&(a1) B(l*r1,w*wr)]/(180)[&(a2) B(l*r2,w*wr)]
B(l,w) => X(l,w)[+(a1)$B(l*r1,w*wr)]      [-(a2)$B(l*r2,w*wr)]
Decomposition Productions:
Homomorphic Productions:
X(l,w) => !(w)F(l)
