//$Header$
//------------------------------------------------------------------------------
//                              PhysicalModel
//------------------------------------------------------------------------------
// *** File Name : PhysicalModel.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/8/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/5/2003 - D. Conway, Thinking Systems, Inc.
//                             Incorporated the Derivative class into this
//                             class and removed Derivative from the class
//                             heirarchy
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated parameter strings to include units;
//                             Added code to switch between relative and 
//                             absolute erro
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/15/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                                - virtual char* GetParameterName(const int parm) const to
//                                  virtual std::string GetParameterName(const int parm) const
//                              Removals:
//                                - static Real parameterUndefined
//                                - SetUndefinedValue()
//                                - ParameterCount()
//                                - GetParameter()
//                                - SetParameter()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Fixed format.
//                              Removals:
//                                - GetParameterName()
//
//                           : 10/23/2003 - D. Conway, Thinking Systems, Inc. &
//                                          W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Changed constructor from PhysicalModel::PhysicalModel(void) to
//                                  PhysicalModel(Gmat::ObjectType typeId, const std::string &typeStr,
//                                  const std::string &nomme = "")
//                                - Added parameterCount = 1 in constructors
//
// **************************************************************************
/** 
 * Base class used to model the physical system
 */

#ifndef PhysicalModel_hpp
#define PhysicalModel_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

#include "ForceModelException.hpp"

#include <math.h>

/** 
 * Base class used to model the physical system
 * 
 * This class is a base class used to model the physics of the system being
 * studied.  The propagators work in tandem with classes derived from this one 
 * to advance the system over time.  
 *
 * Propagators fall into two basic subclasses: Integrators and analytic 
 * solutions.  The analytic solutions typically require minimal interaction with
 * the system; for example, for two body orbit propagation, the PhysicalModel
 * supplies the gravitational constant for the centralbody.  Integrators require
 * more detailed information to evolve their models; see the text of the 
 * PhysicalModelIntegrator class description for details of their requirements.
 */
class GMAT_API PhysicalModel : public GmatBase
{
public:
   PhysicalModel(Gmat::ObjectType typeId, const std::string &typeStr,
                 const std::string &nomme = "");
   virtual ~PhysicalModel(void);

   PhysicalModel(const PhysicalModel&);
   PhysicalModel& operator=(const PhysicalModel&);
    
   //loj: 5/11/04 removed Get/SetBody()   
   //CelestialBody* GetBody();
   //void SetBody(CelestialBody *body);
   //bool SetBody(const std::string &name);

   virtual bool Initialize(void);

   virtual Integer GetDimension(void);
   virtual Real *  GetState(void);
   const Real *    GetDerivativeArray(void);

   virtual void SetDimension(Integer);
   virtual void SetState(const Real * st);

   Real GetErrorThreshold(void) const;
   bool SetErrorThreshold(const Real thold = 0.10);

   virtual void IncrementTime(Real dt);
   virtual Real GetTime(void);
   virtual void SetTime(Real t);

   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1);
   virtual Real EstimateError(Real * diffs, Real * answer) const;
   virtual bool GetComponentMap(Integer * map, Integer order = 1) const;
    
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const Real parm);
   virtual void SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const std::string parm);
   virtual bool StateChanged(bool reset = true);

   // Parameter accessor methods -- inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   
protected:
   /// Number of parameters being modeled
   Integer dimension;
   /// Flag used to tell the readiness of the model for use
   bool initialized;
   /// Flag that is set when SetState() or SetTime() is called
   bool stateChanged;

   /// Array of data parameters containing the model data
   Real *modelState;
   /// The base epoch
   Real epoch;
   /// Number of seconds elapsed from the base epoch
   Real elapsedTime;
   /// Array containing the most recent derivative calculation, when needed
   Real * deriv;
   /// Threshold for switching between relative and absolute error control
   Real relativeErrorThreshold;
   /// Pointer to the solar system model used as a data provider for the forces
   SolarSystem *solarSystem;
   /// Pointer to the origin associated with the force model
   //CelestialBody *theBody; //loj: 5/11/04
   
   /// Parameter IDs
   enum
   {
      EPOCH = GmatBaseParamCount, 
      ELAPSED_SECS,
      PhysicalModelParamCount
   };

   static const std::string PARAMETER_TEXT[PhysicalModelParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[PhysicalModelParamCount];
};

#endif // PhysicalModel_hpp
