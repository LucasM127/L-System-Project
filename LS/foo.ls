Axiom:
!(0.2)A
Skippable Letters:

Productions:
!(x) => !(x+0.1)
B(x) => B(x *1.1)
A => B(0.5)[+!(0.2)A][-!(0.2)A]

Homomorphic Productions:
;B(x) => ~(0.8,0.2,0.1)F(x)
;A => !(0.3)~(0,0.8,0.1)[f(0.4)F(0.2)][+(90)f(0.4)F(0.2)][-(90)f(0.4)F(0.2)]
