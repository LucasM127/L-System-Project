#include "AppViewer.hpp"
#include <iostream>

#include <X11/Xlib.h>

int main()
{//GAH
    XInitThreads();
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