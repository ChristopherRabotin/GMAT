//$Header$
//------------------------------------------------------------------------------
//                             PlotInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/12/18
//
/**
 * Declares PlotInterface class.
 */
//------------------------------------------------------------------------------
#ifndef PlotInterface_hpp
#define PlotInterface_hpp

#include "gmatdefs.hpp"


class PlotInterface
{

public:

    static bool CreatePlotWindow();
    static bool UpdateSpacecraft(const Real &time, const Real &posX,
                                 const Real &posY, const Real &posZ);
    
private:
    
    PlotInterface();
   ~PlotInterface();

};

#endif
