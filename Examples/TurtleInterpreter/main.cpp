#include "AppViewer.hpp"
#include <iostream>

int main()
{//FUCK THIS
    try
    {
        AppViewer app;
        app.run();
    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
        return -1;
    }

    return 0;
}