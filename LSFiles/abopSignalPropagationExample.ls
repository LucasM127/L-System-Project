;Development Controlled by a signal example
;Use line turtle. 30?
;angle 30?

#define d 7 4 iteration delay of signal start
#define m 3 2 iterations for a to generate new internode
#define u 2 1 signal wait time

Axiom:
D(1)a(1)
Skippable Letters:

Productions:
a(i) : i<m => a(i+1)
a(i): i==m => I[L]a(1)
D(i) : i<d => D(i+1)
D(i) :i==d => S(1)
S(i): i<u => S(i+1)
S(i) : i==u =>
S(i) < I : i==u => IS(1)
S(i) < a(j) => I[L]A
A => K
Homomorphic Productions:
I => ~(0.8,0.2,0.1)FF
L => ~(0,0.8,0.1)[+F+F][++F-F][-F-F][--F+F]
A => I~(0.4,0.6,0.3)[+F[F]][-F[F]][F]
K => I~(0.7,0.1,0.4)[+F[F--F]--FF][-F[F++F]++FF]F[+F--F][-F++F]
