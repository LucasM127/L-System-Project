#include "Logger.hpp"

int main()
{
    LOG("This is on its own line");
    LOG_S("This is ");
    LOG_S("on the same line", "\n");

    return 0;
}