;pdf Modeling plant development with L-systems pg 5
;L system modeling the development of a vegetative segment of Anabaena
;W  cell walls with parameter of age
;L long cell
;S short cell

Axiom:
W(0)L(0)W(1)

Skippable Letters:

Productions:
W(a) => W(a+1)
W(l) < L(a) > W(r) : l >= r => S(0)W(0)L(a+1)
W(l) < L(a) > W(r) : l < r => L(a+1)W(0)S(0)
S(a) => L(a+1)

Homomorphic Productions:
