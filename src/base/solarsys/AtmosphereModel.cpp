//$Header$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * Base class for the atmosphere models
 */
//------------------------------------------------------------------------------


#include "AtmosphereModel.hpp"


//------------------------------------------------------------------------------
//  AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const std::string &typeStr) :
    GmatBase            (Gmat::ATMOSPHERE, typeStr),
    fileReader          (NULL),
    sunVector           (NULL),
    centralBody         ("Earth"),
    centralBodyLocation (NULL),
    cbRadius            (6378.14)
{
}


//------------------------------------------------------------------------------
//  ~AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::~AtmosphereModel()
{
}


//------------------------------------------------------------------------------
//  void SetSunVector(Real *sv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the Sun.
 * 
 * @param sv   The Sun vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSunVector(Real *sv)
{
    sunVector = sv;
}


//------------------------------------------------------------------------------
//  void SetCentralBodyVector(Real *cv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the body with the atmosphere.
 * 
 * @param cv   The body's position vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBodyVector(Real *cv)
{
    centralBodyLocation = cv;
}

//------------------------------------------------------------------------------
//  void SetSolarFluxFile(std::string file)
//------------------------------------------------------------------------------
/**
 * @param file The solar flux file
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarFluxFile(std::string file)
{

}

//------------------------------------------------------------------------------
//  AtmosphereModel(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const AtmosphereModel& am) :
GmatBase            (am),
fileReader          (NULL),
sunVector           (NULL),
centralBody         (am.centralBody),
centralBodyLocation (NULL),     // ************ NULL?
cbRadius            (am.cbRadius)
{
   
}


