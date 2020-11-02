#define VARIATE 20-rand()%40  5-rand()%10
#define LEAFCOLOR 0.3+(rand()%40)/100.f,0.8+(rand()%15)/100.f,0.3+(rand()%40)/100.f (rand()%40)/100.f,0.8+(rand()%15)/100.f,(rand()%40)/100.f
#define TRUNKCOLOR 0.4+(rand()%15)/100.f,0.8+(rand()%20)/100.f,0.2+(rand()%20)/100.f more GREEN
;depth factor, x (can't have branches upon branches)
;length of branch, l
;branch spacing factor, k
#define BRANCHING_ANGLE 60 was 30
#define MAX_BRANCH_LENGTH 2
#define LA 25
#define INTERNODE ~(TRUNKCOLOR)+(VARIATE)I
;#define LEAF ~(LEAFCOLOR)B

Axiom:
!(0.01)A(0,0,0)

Skippable Letters:
+-
Productions:
;It is matching the products to the production, but choosing them is not working out...
A(x,l,k) : x > 0.01 && l > MAX_BRANCH_LENGTH => : 50
A(x,l,k) : l > 19 => : 16

;A(x,l,k) : l < 20 => I[+(45)B(0)][-(45)B(0)]A(x,l+1,k+1) : 30

;A(x,l,k) => I[+(25)XB(0)][-(25)XB(0)]A(x,l+1,k+1) : 30
A(x,l,k) => INTERNODE[+(25)XB(0)][-(25)XB(0)]A(x,l+1,k+1) : 30

;branching if x < 1
;make the leaves more level with the ground?
;A(x,l,k) : x < 0.99 && k > 1 && l < 20 => I[+(25)XB(0)][-(25)XB(0)]/(137)[-(BRANCHING_ANGLE)A(x+1,0,0)]A(x,l+1,0) : 2

;A(x,l,k) : x < 0.99 && k > 2 => I[+(25)XB(0)][-(25)XB(0)][-(BRANCHING_ANGLE)A(x+1,0,0)]A(x,l+1,0) : 2
;A(x,l,k) : x < 0.99 && k > 2 => I[+(25)XB(0)][-(25)XB(0)][+(BRANCHING_ANGLE)A(x+1,0,0)]A(x,l+1,0) : 2
A(x,l,k) : x < 0.99 && k > 2 => INTERNODE[+(25)XB(0)][-(25)XB(0)][-(BRANCHING_ANGLE)A(x+1,0,0)]A(x,l+1,0) : 2
A(x,l,k) : x < 0.99 && k > 2 => INTERNODE[+(25)XB(0)][-(25)XB(0)][+(BRANCHING_ANGLE)A(x+1,0,0)]A(x,l+1,0) : 2
X => % : 1
X => : 3

Decomposition Productions:
G(x) : x < 10 = > F(0.01)[+(90*cosf(x/6))f(0.01)F(0.02*sinf(x/3))][-(90*cosf(x/6))f(0.01)F(0.02*sinf(x/3))]G(x+1)
;G(x) : x < 10 => +(VARIATE)T(-5)!(0.001*(10-x))F(0.05)G(x+1)
B(x) => ~(LEAFCOLOR)!(0.006)G(x)
Homomorphic Productions:
;G(x) : x < 10 = > F(0.01)[+(90*cosf(x/6))f(0.01)F(0.02*sinf(x/3))][-(90*cosf(x/6))f(0.01)F(0.02*sinf(x/3))]G(x+1)
;G(x) : x < 10 => +(VARIATE)T(-1)!(0.001*(10-x))F(0.05)G(x+1)

;G(x) : x < 10 => +(VARIATE)T!(0.001*(10-x))F(0.05)G(x+1)
;B(x) => ~(LEAFCOLOR)!(0.006)G(x)

I => /(137)!(0.01)T(3)F(0.07)
;I => /(137)!(0.01)~(TRUNKCOLOR)+(VARIATE)T(-5)F(0.07)
;I => /(137)!(0.01)~(TRUNKCOLOR)+(VARIATE)T(-8)f(0.01)F(0.05)f(0.01)
