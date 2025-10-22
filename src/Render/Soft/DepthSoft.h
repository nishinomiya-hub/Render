#pragma once

#include "Base/PipelineStates.h"

namespace Learn
{
    bool DepthTest(float &a, float &b, DepthFunction func)
    {
        switch (func)
        {
        case ALWAYS:
            return true;
        case EQUAL:
            return  std::fabs(a - b) <= std::numeric_limits<float>::epsilon();
        case GEQUAL:
            return a >= b;
        case GREATER:
            return a > b;
        case LEQUAL:
            return a <= b;
        case LESS:
            return a < b;
        case NEVER:
            return false;
        case NOTEQUAL:
            return std::fabs(a - b) > std::numeric_limits<float>::epsilon();
        }
        return a < b ;
    }
}