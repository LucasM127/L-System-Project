#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

//<A>:=>A
float func_0(const float *V)
{
float s = V[0];
return s*5;
}

//<A>:s>0.2=>AA
float func_1(const float *V)
{
float s = V[0];
return s/2;
}

//<A>:s>0.2=>AA
float func_2(const float *V)
{
float s = V[0];
return s/2;
}

//A(s):s>0.2=>A(s/2)A(s/2)
float func_3(const float *V)
{
float s = V[0];
return s>0.2;
}

//<A>:=>fFf
float func_4(const float *V)
{
float s = V[0];
return 0.01;
}

//<A>:=>fFf
float func_5(const float *V)
{
float s = V[0];
return s-0.02;
}

//<A>:=>fFf
float func_6(const float *V)
{
float s = V[0];
return 0.01;
}

#ifdef __cplusplus
}
#endif
