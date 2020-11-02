#include "LSLoader.hpp"

int main()
{
    LSYSTEM::LSLoader loader;
    LSYSTEM::LSystem *L = loader.loadFile("foo.ls");

    Text_Turtle T(std::cout);

    T.init();
    L->feedTurtle(T);
    T.draw();

    for(int i = 0; i < 2; ++i)
    {
        T.init();
        L->iterate();
        L->feedTurtle(T);
        T.draw();
        
        std::cout<<std::endl;
    }

    return 0;
}