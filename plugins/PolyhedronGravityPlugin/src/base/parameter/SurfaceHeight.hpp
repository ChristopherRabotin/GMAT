//------------------------------------------------------------------------------
//                           SurfaceContact
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 30, 2016
/**
 * Parameter used to detect contact on the central body in a force model
 */
//------------------------------------------------------------------------------

#ifndef SurfaceContact_hpp
#define SurfaceContact_hpp

#include "polyhedrongravitymodel_defs.hpp"
#include "GravReal.hpp"

/**
 * Class used to stop on a Celestial Body's surface.
 */
class POLYHEDRONGRAVITYMODEL_API SurfaceHeight : public GravReal
{
public:
   SurfaceHeight(const std::string &name = "", GmatBase *obj = NULL);
   virtual ~SurfaceHeight();
   SurfaceHeight(const SurfaceHeight& sc);
   SurfaceHeight& operator=(const SurfaceHeight& sc);

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

#endif /* SurfaceContact_hpp */
