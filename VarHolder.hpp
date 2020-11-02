#ifndef VARHOLDER_HPP
#define VARHOLDER_HPP

namespace LSYSTEM
{

struct VarIndice
{
    VarIndice() : letterNum(0), paramNum(0) {}
    VarIndice(unsigned int a, unsigned int b) : letterNum(a), paramNum(b) {}
	unsigned int letterNum;
	unsigned int paramNum;
};

template <class VALUE>
struct Array2D_t
{
    Array2D_t(unsigned int _width, unsigned int _depth) : width(_width), depth(_depth), vals(new VALUE(width * depth)){}
    ~Array2D_t(){ delete[] vals; }
    VALUE &operator[](unsigned int index) { return vals + (index * depth); }
    VALUE &operator()(const VarIndice &VI) { return vals[VI.letterNum * depth + VI.paramNum]; }
    //bool isValid(VarIndice &VI)
    //const VALUE & const operator[](unsigned int index) const { return vals + (index * depth); }
    const unsigned int width, depth;
    VALUE *vals;
};

typedef Array2D_t<float> Array2D;

} //namespace LSYSTEM

#endif //VARHOLDER_HPP