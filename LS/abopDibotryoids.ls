#define dd 13 delay for sending florigen
#define ee 3
#define m 2
#define nn 3
#define u 1
#define v 1

Axiom:
S(0)a(1,0)
Skippable Letters:
Productions:
a(t,c) : t<m => a(t+1,c)
a(t,c) : (t>=m)&&(c < ee) => I(0,u)[L] a(1,c+1)
a(t,c) : (t>=m)&&(c>=ee)  => I(0,u)[L][b(1)] a(1,c)

b(t) : t<nn => b(t+1)
b(t) : t>=nn => I(0,v)[L]b(1)

S(t) => S(t+1)

S(t) < I(i,j) : t>=dd => I(1,j)
I(i,j) : (0<i)&&(i<j) => I(i+1,j)
I(i,j) < I(k,l) : (i==j)&&(k==0) => I(1,l)
I(i,j) < a(k,l) : i > 0 => I(i,j)[L][b(1)]B
I(i,j) < b(k) : i > 0 => I(i,j)[L]B
B => I[K]B
Decomposition Productions:
Homomorphic Productions:
I(i,j) => !(0.2)~(1,0,0)F(1)
;I internode
L => !(0.1)~(0,1,0)+(45)F(0.2)-(45)F(0.2)-(45)F(0.2)-(90)F(0.2)-(45)F(0.2)-(45)F(0.2)
;L leaf
K => !(0.4)~(1,0,1)F(0.4)
;K flowers
A => !(0.3)~(0,1,1)F(0.3)
;A flowering apex
;a apex (no show)

