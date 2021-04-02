Globals:
a 50
b 25
c 10
Axiom:
K(50)
;A(50)
Productions:
K(x) : x >=20=> B/(137)[^(80)A(20)]K(x-1)
K(x) : x<20 => B/(137)[^(80)A(x)]K(x-1)
K(x) : x==0 =>
A(x) : x>b       => [/(15)P(0,a-x)]/(180)[/(-15)P(0,a-x)]/(180)BA(x-1)
A(x) : x<=b&&x>a-b => [/(15)P(0,a-b)]/(180)[/(-15)P(0,a-b)]/(180)BA(x-1)
A(x) : x==0      =>
A(x) : x>0&&x<=a-b => [/(15)P(0,x)]/(180)[/(-15)P(0,x)]/(180)BA(x-1)

P(x,y) : x<y     => P(x+1,y)
Homomorphic Productions:
P(x,y) : x > 0 => ~(1-0.5x/c,x/c,0)+(5)FP(x-1,y)
B => ~(0.5,0.4,0.2)F

;x == 0 and x<=5 both evaluate to true.
;Had to put x=0 first for it to be tested though
;is that a concern?
