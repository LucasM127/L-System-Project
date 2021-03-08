#ifndef LSYSTEM_INTERPRETER_DATA_HPP
#define LSYSTEM_INTERPRETER_DATA_HPP

struct LSystemTurtleInterpreterData
{
    float turn = 90;//default turning angles
    float pitch = 90;
    float roll = 90;
    float tx = 0;//default tropism vector
    float ty = 1;
    float tz = 0;
    float thickness = 0.1;
    void reset()
    {
        turn = 90;
        pitch = 90;
        roll = 90;
        tx = 0;
        ty = 1;
        tz = 0;
        thickness = 0.1;
    }
};

#endif //LSYSTEM_INTERPRETER_DATA_HPP