;pg 60 Modeling of trees ABOP pdf
#define d1 180 divergence angle 1
#define d2 252 divergence angle 2
#define a 36 branching angle
#define lr 1.07 elongation rate
#define vr 1.432 width increase rate

Axiom:
!(1)F(200)/(45)A

Skippable Letters:
Productions:
A => !(vr)X(50)[&(a)X(50)A]/(d1)[&(a)X(50)A]/(d2)[&(a)X(50)A]
X(l) => X(l*lr)
!(w) => !(w*vr)
Decomposition Productions:
Homomorphic Productions:
X(l) => F(l)T(22.918)
