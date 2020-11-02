;comment
#define c 1
#define z 0.3
#define q (c-z)
;#define h (z*q)^0.5
#define h powf(z*q,0.5)

Axiom:
A(5,0)
Skippable Letters:

Productions:
A(x,t):t==0 => A(x*z,2)+A(x*h,1)--A(x*h,1)+A(x*q,0)
A(x,t):t>0 => A(x,t-1)
;A(x,t)>B(c) => A(x,t)
;B(x) => B(x)
Homomorphic Productions:
A(x,t) => F(x,t)
;B(x) => f(x)
;not reading the , at all... A(xt) same as A(x,t)... we can change that to create a string p'haps
