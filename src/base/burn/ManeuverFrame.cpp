//$Header$
//------------------------------------------------------------------------------
//                              ManeuverFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the base class for maneuver reference frames. 
 */
//------------------------------------------------------------------------------


#include "ManeuverFrame.hpp"
#include <math.h>


//------------------------------------------------------------------------------
//  ManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Constructs the maneuver frame object (default constructor).
 */
//------------------------------------------------------------------------------
ManeuverFrame::ManeuverFrame() :
   position        (NULL),
   velocity        (NULL)
{
   basisMatrix[0][0] = basisMatrix[1][1] = basisMatrix[2][2] = 1.0;
   basisMatrix[0][1] = basisMatrix[0][2] = basisMatrix[1][0] =
   basisMatrix[1][2] = basisMatrix[2][0] = basisMatrix[2][1] = 0.0;
}


//------------------------------------------------------------------------------
//  ~ManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Destroys the maneuver frame object (destructor).
 */
//------------------------------------------------------------------------------
ManeuverFrame::~ManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  ManeuverFrame(const ManeuverFrame& mf)
//------------------------------------------------------------------------------
/**
 * Constructs the maneuver frame object (copy constructor).
 * 
 * @param <mf> Object that is copied.
 */
//------------------------------------------------------------------------------
ManeuverFrame::ManeuverFrame(const ManeuverFrame& mf)
{
}


//------------------------------------------------------------------------------
//  ManeuverFrame& operator=(const ManeuverFrame& mf)
//------------------------------------------------------------------------------
/**
 * Sets one maneuver frame object to match another (assignment operator).
 * 
 * @param <mf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
ManeuverFrame& ManeuverFrame::operator=(const ManeuverFrame& mf)
{
   if (this == &mf)
      return *this;
        
   return *this;
}


//------------------------------------------------------------------------------
//  void SetState(Real *pos, Real *vel)
//------------------------------------------------------------------------------
/**
 * Sets the pointers for the state data that need to incorporate the maneuver.
 * 
 * @param <pos> The starting address of the position data.
 * @param <vel> The starting address of the velocity data.  If this pointer is
 *              NULL, the code interpretes pos as a pointer to position data 
 *              followed by velocity data.
 */
//------------------------------------------------------------------------------
void ManeuverFrame::SetState(Real *pos, Real *vel)
{
   position = pos;
   if (vel)
      velocity = vel;
   else
      velocity = pos + 3;
}


//------------------------------------------------------------------------------
//  void CalculateBasis(Real basis[3][3])
//------------------------------------------------------------------------------
/**
 * Fills the input matrix with the orthonormal basis vectors used for the 
 * maneuver frame.
 * 
 * This method calls the protected CalculateBasis(void) method to perform the 
 * actual calculations.
 * 
 * @param <basis> The matrix that gets filled with the data.
 */
//------------------------------------------------------------------------------
void ManeuverFrame::CalculateBasis(Real basis[3][3])
{
   CalculateBasis();
    
   Integer i, j;
   for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
         basis[i][j] = basisMatrix[i][j];
}


//------------------------------------------------------------------------------
//  std::string GetFrameLabel(Integer id)
//------------------------------------------------------------------------------
/**
 * Gets the label for the maneuver frame principal axes.
 * 
 * @param <id> Integer ID of the axis -- 1, 2, or 3.
 * 
 * @return The label for the corresponding reference vector.
 */
//------------------------------------------------------------------------------
std::string ManeuverFrame::GetFrameLabel(Integer id)
{
   switch (id)
   {
      case 1:
         return "X";
      case 2:
         return "Y";
      case 3:
         return "Z";
      default:
         ;
   }
   return "Undefined";
}
