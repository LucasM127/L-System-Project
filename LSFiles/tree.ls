#define WIDTH 0.04
#define ANGLE1 20+rand()%30-15
#define ANGLE2 ANGLE1 10+rand()%10
#define ANGLE3 rand()%40-20
#define VARIATE1 0 float(rand()%100)/1000
#define VARIATE2 float(rand()%400)/1000

Axiom:
;!(0.4)A
!(WIDTH)A
Skippable Letters:
X%!&^+-
Productions:
;A => I(1)[+A][-A]I(1)A
;I(s) => I(2*s)

;F(x) => G(x)[+F(0.7*x)][-F(0.7*x)]
;G(x) => G(1.2 * x)
;!(x) => !(x+1)
;F(x) => F(x*1.1)
!(x) => !(x+WIDTH)
B(x) => B(x *1.1)
A => B(0.7)[/(ANGLE1)^(ANGLE3)+(ANGLE1)!(WIDTH)XA][\(ANGLE1)^(ANGLE3)-(ANGLE1)!(WIDTH)XA][^(ANGLE2)!(WIDTH)XA]:8

;A => B(0.7)[^(ANGLE3)+(ANGLE1)!(WIDTH)XA][^(ANGLE3)-(ANGLE1)!(WIDTH)XA][^(ANGLE2)!(WIDTH)XA]:1
;A => B(0.7)[^(ANGLE3)+(ANGLE1)!(WIDTH)XA][^(ANGLE3)-(ANGLE1)!(WIDTH)XA][^(ANGLE2)!(WIDTH)XA]:1
A => B(0.7)[&(ANGLE2)+(ANGLE1)!(WIDTH)XA][^(ANGLE2)-(ANGLE1)!(WIDTH)XA]:4
A => B(0.7)[^(ANGLE2)+(ANGLE1)!(WIDTH)XA][&(ANGLE2)-(ANGLE1)!(WIDTH)XA]:4
A => B(0.7)[&(ANGLE2)+(ANGLE1)!(WIDTH)XA][&(ANGLE2)-(ANGLE1)!(WIDTH)XA]:4
A => B(0.7)[^(ANGLE2)+(ANGLE1)!(WIDTH)XA][^(ANGLE2)-(ANGLE1)!(WIDTH)XA]:4
A => B(0.7)[^(ANGLE2)+(ANGLE1)!(WIDTH)XA]:2
A => B(0.7)[&(ANGLE2)+(ANGLE1)!(WIDTH)XA]:2
A => B(0.7)[^(ANGLE2)-(ANGLE1)!(WIDTH)XA]:2
A => B(0.7)[&(ANGLE2)-(ANGLE1)!(WIDTH)XA]:2
X => X : 1
X => /(ANGLE3)A% : 2
Decomposition Productions:
Homomorphic Productions:
B(x) => ~(0.8+VARIATE1,0.2+VARIATE1,0.1+VARIATE1)F(x)
;A => !(0.3)~(0,0.8,0.1)[f(0.4)F(0.2)][+(90)f(0.4)F(0.2)][-(90)f(0.4)F(0.2)][^(90)f(0.4)F(0.2)][&(90)f(0.4)F(0.2)]
A => !(0.3)~(VARIATE2,0.3+VARIATE2,0.1+VARIATE2)[f(0.4)F(0.2)][+(90)f(0.4)F(0.2)][-(90)f(0.4)F(0.2)][^(90)f(0.4)F(0.2)][&(90)f(0.4)F(0.2)]
;G => X
;G(x) => F(x)
;A => ~(0,1,0)f(0.2)F(0.8)
;I(s) => ~(1,0.2,0.1)f(0.2)F(s-0.2)
