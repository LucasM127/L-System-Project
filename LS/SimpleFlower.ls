;I internode
;K flower
;L leaf

#define Ta 7 developmental switch time
#define Tl 9 leaf growth limit
#define Tk 5 flower growth limit

Axiom:
a(1)
Skippable Letters:
Productions:
a(t) : t < Ta => I(1)[L(0)]/(137.5)a(t+1)
a(t) : t >= Ta => I(20)A
A => K(1)
L(t) : t < Tl => L(t+1)
K(t) : t < Tk => K(t+1)
I(l) : l < 2 => I(l + 0.2)
Decomposition Productions:
Homomorphic Productions:
I(l) => !(0.2)~(0.4,1.0,0)[/(90)F(0.1*l)]F(0.1*l)
;I internode
L(t) => &(60)!(0.2)~(0,1,0)+(45)F(0.2*t)-(45)F(0.2*t)-(45)F(0.2*t)-(90)F(0.2*t)-(45)F(0.2*t)-(45)F(0.2*t)
;L(t) => &(60)T(8)!(0.2)~(0,1,0)F(0.2*t)[+(60)F(0.3*t)][-(60)F(0.3*t)]F(0.2*t)[+(60)F(0.2*t)][-(60)F(0.2*t)]F(0.2*t)[+(60)F(0.1*t)][-(60)F(0.1*t)]
;L(t) => &(60)!(0.2)~(0,1,0)G(t)
;G(t) : t > 0 => F(0.1*t)[+(90*cosf(t/6))^(10)f(0.1)F(0.3*t*sinf(t/3))][-(90*cosf(t/6))^(10)f(0.1)F(0.3*t*sinf(t/3))]G(t-1)
;L leaf
K(t) => !(0.5*t)~(1,0,1)[/(90)F(0.5*t)]F(0.5*t)
;K flowers
A => !(0.5)~(0,1,1)[/(90)F(0.5)]F(0.5)
;A flowering apex
;a apex (no show)

