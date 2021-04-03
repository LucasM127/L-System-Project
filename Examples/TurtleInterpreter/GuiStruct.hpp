#include <vector>
#include <string>

#define RANGE(...) LabelType()

struct LabelType
{
    LabelType(){}
};

struct GuiStructIdea
{
    enum class Label : uint
    {
        TurnAngle = 0,
        PitchAngle,
        RollAngle,
        TropismVec,
        Tropism
    };
    std::vector<std::string> labelStrings = 
    {
        "Turn Angle",
        "Pitch Angle",
        "Roll Angle",
        "Tropism Vec",
        "Tropism"
    };
    //map each 'label to a gui - type constructor...
    std::vector<LabeLType> labelTypes = 
    {
        RANGE(0-180),
        RANGE(0-180),
        RANGE(0-180),
        RANGE(0-1),RANGE(0-1),RANGE(0-1),
        RANGE(0-180)
    };
};

struct TI_Info
{
    std::vector<string> labels;
    float TurnAngle = 90.f;
    float PitchAngle = 90.f;
    float RollAngle = 90.f;
    glm::vec3 m_defaultTropismVector = glm::vec3(0,-1,0);
    float m_defaultTropismAmount;//FOrget what this does.
};