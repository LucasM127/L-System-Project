;naive model, go straight till hit an obstacle
;each created segment may branch dependant on facing direction to simulate tropism

Axiom:
S

Productions:
S => A

A => IA

;up can only branch right
w => I
w => B : 2
w => B[+A] :1

;shouldn't be heading down can only branch left
s => I
s => B :2
s => B[-A]:1

;left only can branch right to go up
a => I
a => B:2
a => B[+A]:1

;right only can branch left
d => I
d => B:2
d => B[-A]:1

X => x

Homomorphic Productions:
B => mF
I => mF
A => mxf
X => mx

;Turtle Commands (hardcoded)
;m: mark the environment as occupied at current turtle position
;F: draw full branch segment at current turtle position
;x: draw branch end segment at current turtle position
;f: advance turtle 1 unit forward
;+: turn right
;-: turn left

;Environmental Program (hardcoded)
;A: replace with X if current turtle position is occupied
;I: replace with 'w' or 'a' or 's' or 'd' dependent an current turtle heading