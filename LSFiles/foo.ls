;Development Controlled by a signal example
;Use line turtle.
;angle 30?

#define d 7 4 iteration delay of signal start
#define m 3 2 iterations for a to generate new internode
#define u 2 1 signal wait time

Axiom:
A(0)
Skippable Letters:

Productions:
A(x) => f(0.2)[&(120-x)L(1,x)]/(137)A(x+1)
A(x) : x>90 =>
L(x,y) : y < 180 => L(x*1.01,y+1)
Homomorphic Productions:
L(x,) => ~(x/2,x,1-(x/2))[+F(x)--F(x)][-F(x)++F(x)]
