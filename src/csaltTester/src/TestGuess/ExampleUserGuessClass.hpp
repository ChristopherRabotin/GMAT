// This is an example of a user guess class

#ifndef ExampleUserGuessClass_hpp
#define ExampleUserGuessClass_hpp

#include "TrajectoryData.hpp"


class ExampleUserGuessClass : public TrajectoryData
{
public:
    ExampleUserGuessClass() :
        TrajectoryData()
    {
        numStateParams = 2;
        numControlParams = 1;
        numIntegralParams = 0;
    };
    
    Rmatrix GetState(Rvector requestedTimes)
    {
        Rmatrix output;
        output.SetSize(requestedTimes.GetSize(),numStateParams);
        for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
            for (Integer jdx = 0; jdx < numStateParams; jdx++)
                output(idx,jdx) = 1.0;
        return output;
    };
    Rmatrix GetControl(Rvector requestedTimes)
    {
        Rmatrix output;
        output.SetSize(requestedTimes.GetSize(),numControlParams);
        for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
            for (Integer jdx = 0; jdx < numControlParams; jdx++)
                output(idx,jdx) = 0.0;
        return output;
    };
    Rmatrix GetIntegral(Rvector requestedTimes)
    {
        Rmatrix output;
        output.SetSize(numIntegralParams,requestedTimes.GetSize());
        for (Integer idx = 0; idx < requestedTimes.GetSize(); idx++)
            for (Integer jdx = 0; jdx < numIntegralParams; jdx++)
                output(jdx,idx) = 1.0;
        return output;
    };

    Integer numStateParams;
    Integer numControlParams;
    Integer numIntegralParams;
};

#endif