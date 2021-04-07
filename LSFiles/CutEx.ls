;Cut Symbol EXAMPLE

Axiom:
!(0.1)A
Skippable Letters:
Productions:
!(x) => !(x+0.1)
G(x) => G(x*1.1)
A => G(1)[+(25+rand40)!(0.1)XA][-(25)!(0.1)XA]
X => X : 1
X => A% : 1
Homomorphic Productions:
G(x) => ~(0.6,0.2,0.1)F(x)
