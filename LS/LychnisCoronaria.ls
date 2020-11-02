;TODO; how to initiate an L System with access to a global variable T
;each iteration 
;T => T+dt
;is an evaluated number, save as a GLOBAL Variable
;like with the other defines, then can change in run time, instead of just hit and switch
;ie #global T(n) = 0 + n*DT
;send to our ... in the array, but at the LAST POSITION, add to all the varindicemaps float V[..variables.. ..globals..]
;or just iterate it x times till we are happy or just pretend T
;pg 16+ Modeling plant development with L-systems pdf
;repetitive sypodial branching
;architecture apices (apical meristems), internodes, leaves, flowers and fruits
;each axis is terminated by a flower
;Modules
;A(1) apex(age), I(3) internode(target length, width, age), L(1) leaf(age), K(1) bud/flower/fruit(age), B(1) branching point (age)
;plant components but branching point is -> specify time-varying branching angles age(t) parameterized
#define DT 1
;overall plant age, t+DT of the plant, global variable
#define T 0 => T += DT 
#define BRANCHING_ENDS 10
#define LEN1
#define LEN2
#define PHI
#define D1 -1
#define D2 -2

;Initial shoot IK
Axiom:
I(1,0.1,0)A(-1)
Skippable Letters:
Productions:
A(t) => A(t + DT)
L(t) => L(t + DT)
K(t) => K(t + DT)
B(t) => B(t + DT)
;uses >> 'fast information transfer, variant of context sensitivity that makes possible to propagate info in onve direction (extremeties -> base ie) in a single step
I(l,w,t) > [I(l1,w1,t1)][I(l2,w2,t2)]I(l3,w3,t3] => I(l, w1+w2+w3, t+DT)
I(l,w,t) > [I(l1,w1,t1)]I(l2,w2,t2) => I(l, w1+w2, t+DT)
I(l,w,t) > I(l1,w1,t1) => I(l,w1,t+DT)
I(l,w,t) > K(t1) => I(l, width(t), t+DT)
Decomposition Productions:
;apex reaching threshold age of 0 D2<D1<0 branches initiated after unequal delyas
A(t) : t > 0 && T < BRANCHING_ENDS => I(LEN1, 0, t)[/(PHI)B(t)[L(t)]A(D1+t)][\(PHI)B(t)[L(t)]A(D2+t)]I(LEN2,0,t)K(t)
;creates a flowering bud -> open flower -> mature seed pod a homomorphism of K for instance
Homomorphic Productions:
B(t) => DOWN(f(t)) //turn the branch down as a function of time
L(t) => down(leaf_angle(t))RolltoVert()CurrentTexture(LEAF_TEXTURE)SetColor(LeafColor(t))Surface(LEAF_SURFACE, leaf_length(t))
I(l,w,t) => setColor(branch_color(T))setWidth(pow(w, 1/eta)-- for(i 0-NSEG) produce F(l*int_length(t) / NSEG)
;subdivide the length into smaller sections to simulate ie, gravitropism.  (ie each f turtle, rotate down a tiny bit f(width) or someting like tht
;Could probably use recursion for this...
;diameter (width) d = w^(1/n)


