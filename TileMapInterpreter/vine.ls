Axiom:
S

Productions:
S => A
S => [+A]A
S => [-A]A
S => [+A][-A]A

A => IA : 16
A => I+A : 8
A => I-A : 8
A => I[+A]A : 2
A => I[-A]A : 2
A => I[+A][-A]A : 1
A => A : 32

Homomorphic Productions:
I => mF
A => mxf
X => mx

;Turtle Commands
;m: mark the environment as occupied at current turtle position
;F: draw full branch segment at current turtle position
;x: draw branch end segment at current turtle position
;f: advance turtle 1 unit forward
;+: turn right
;-: turn left

;Environmental Program queries current turtle position at A or B
;and replaces with an X if the environment is occupied at queried position
